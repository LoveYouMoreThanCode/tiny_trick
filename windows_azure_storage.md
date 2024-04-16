# 2. Global Partitioned Namespace
```
http(s)://AccountName.<service>.core.windows.net/PartitionName/ObjectName
```
- AccountName: DNS域名，用以定位存储集群和数据中心
- PartitionName：用于和AccountName定位存储集群
- ObjectName：可选的。当PartitionName包含多个对象的时候，用以区分对象。系统支持PartitionName相同的对象原子操作。
- Blobs只使用PartitionName；Table使用PartitionName + ObjectName表示一行；Queue使用PartitionName表示queue，ObjectName表示一个消息。
# 3. High Level Architecture 
## 3.1 Windows Azure Cloud Platform
**Fabric Controller**
- 节点管理
- 网络配置
- 服务启停
- 服务部署
**WAS**
- 数据分布
- 数据复制
- 数据均衡
## 3.2 WAS Architectural Components
![image](https://github.com/LoveYouMoreThanCode/tiny_trick/assets/141383156/fa305ae4-1bc4-46d6-a650-cecb4c9ae5c0)
**Storage Stamps**
- 10-20 racks, 18 servers/rack
- 2PB (30PB future)
- 利用率>=70%, 包括容量、事务、带宽；<= 80%, 磁盘内道性能和rack故障容忍的考虑
**Location Service**
- 负责管理所有的Storage Stamps
- 自身需要跨region容灾
- 分配storage stamps访问关系：更新dns，https://AccountName.service.core.windows.net/ --> storage stamp’s virtual IP
## 3.3 Three Layers within a Storage Stamp
**Stream Layer** 
- 知道如何存和如何复制，不知道具体语义
**Partition Layer**
- 负责高一些层级的数据抽象，比如blob、table、queue
- 提供可伸缩的namespace
- 提供事务和强一致性对象
- 存储对象data到stream layer
- 缓存数据，减少对stream layer访问
- 提供扩展性，根据PartitionName做hash打散和分布
**Front-End (FE) layer –**
- 验证、授权、路由（网关作用）
- 对于大对象可以直接访问stream layer
- 支持数据缓存
## 3.4 Two Replication Engines 
**Intra-Stamp Replication（stream layer）**
- IO路径同步复制
**Inter-Stamp Replication (partition layer)**
- 异步复制
# 4. Stream Layer
API list
```
open, close, delete, rename, read, append, concatenate
```
**Stream** : 一个由extent指针组成的有序列表，extent由一系列的append blocks组成
![image](https://github.com/LoveYouMoreThanCode/tiny_trick/assets/141383156/92de9a83-8f27-4666-a8ca-2105c0f78784)
**Block**
- 最小读写单元，最大可以设置到4MiB。
- 读的时候需要按照block对齐读，因为需要获取checksum。
**Extent**
- Stream Layer的复制单元
- 缺省3副本
- NTFS file
- 1GB
**Streams**
## 4.1 Stream Manager and Extent Nodes
![image](https://github.com/LoveYouMoreThanCode/tiny_trick/assets/141383156/455b3b62-b281-4cd2-9416-edf969f6b520)
**Stream Manager (SM)**：保存stream namespace，stream由哪些extent组成 以及 extent的位置信息，是一个paxos集群
- 维护所有stream和extent状态
- 监控所有EN节点健康
- 向EN节点创建和分配extent
- 后台修复复制
- 孤儿extent收集和删除
- Erasue Code编码调度
- 32GB，50 million extents， 100000 streams
**Extent Nodes (EN)**:只知道extents和block
- EN节点记录SM下发的视图信息，用以下发client的请求到replicas
- EN节点负责SM下发的修复Extent任务
- EN节点负责SM下发的删除过期Extent的任务
## 4.2 Append Operation and Sealed Extent
- extent大小不需要一样
- 支持multi-block automic append
- client需要能够处理重复记录
## 4.3 Stream Layer Intra-Stamp Replication
- 一旦写成功，任何一个副本都可以看到相同的数据
- 一旦extents被sealed，后续这个extents总是看到相同的数据
### 4.3.1 Replication Flow
![image](https://github.com/LoveYouMoreThanCode/tiny_trick/assets/141383156/116af736-e612-4f2d-a4b4-30b1dba58c70)
- IO流程client到primary EN，primary EN到secondary EN，无需lease，primary不可改变
- 只写主，但是读可以读任意副本
- 主extent职责：确定offset、排序、发送请求到备、当全部成功返回成功。保序返回
- stream open的时候，client缓存相关extent信息
- extent写失败，向SM请求seal掉，分配新的extent
### 4.3.2 Sealing
- 选择能获取到最小length作为commit length
### 4.3.3 Interaction with Partition Layer
**Read records at known locations**
- row and blob
- extent+offset, length
**Iterate all records sequentially in a stream on partition load.**
- metadata and commitlog
- Partition Layer和Stream Layer一起保证数据一致
## 4.4 Erasure Coding Sealed Extents
- 降低存储冗余到1.x
## 4.5 Read Load-Balancing
- deadline read，deadline内完成不了应该立刻返回
- EC和副本均支持
## 4.6 Spindle Anti-Starvation
- 读写公平，避免饿死
## 4.7 Durability and Journaling
- 使用SSD作为journal加速
- 降低写延时，减少和读抢磁头
# 5. Partition Layer
- 为不同的存储对象提供不同数据模型
- 处理不同对象的逻辑和语义
- 扩展对象命名空间
- partition servers间做负载均衡
- 事务排序和强一致性存储
## 5.1 Partition Layer Data Model
### 5.1.1 Supported Data Types and Operations
## 5.2 Partition Layer Architecture
![image](https://github.com/LoveYouMoreThanCode/tiny_trick/assets/141383156/c325f434-0094-41bc-b51a-dbdfcd881872)
**Partition Manager (PM)**
- 拆分RangePartitions，分配RangePartitions到partition servers
- 负载均衡
**Partition Server (PS)**
- RangePartition只会在一个PS访问，提供强一致性和排序
**Lock Service**
- A Paxos Lock Service
- Partition Server故障，负责的RangePartitions需要转移到其他Partition Servers
## 5.3 RangePartition Data Structures
### 5.3.1 Persistent Data Structure
![image](https://github.com/LoveYouMoreThanCode/tiny_trick/assets/141383156/9a1019b7-9d69-418c-972b-7b404ef63af5)
- LSM like
- RangePartition占用的stream都是独占的
- Partition分裂会导致extent会被多个stream指向
**Metadata Stream –**
- Root Stream for RangePartition
**Commit Log Stream**
- 存储RangePartition的增删改查
**Row Data Stream**
- row data checkpoint
- RangePartition的索引
**Blob Data Stream**
- 存储blob数据
### 5.3.2 In-Memory Data Structures
- Memory Table
- Index Cache
- Row Data Cache
- Bloom Filters
## 5.4 Data Flow
## 5.5 RangePartition Load Balancing 
- Load Balance
- Split
- Merge
### 5.5.1 Load Balance Operation Details 
- Transactions per seconds
- pending的Transactions个数
- 限流速率
- CPU利用率
- 网络利用率
- 请求延时
- data size
### 5.5.2 Split Operation
### 5.5.3 Merge Operation
## 5.6 Partition Layer Inter-Stamp Replication























