# Design Overview
## 假设
- 由大量的廉价机器部件组成。故障是常见的，自动检测和自动恢复是必要的。
- 期望文件组成：少量的大文件。组要文件为几百个100MB+的文件。GB级别的文件是比较常见的，需要优化处理。支持小文件，但是不会做针对性优化。
- 支持大块流式读和小块随机读。大块通常指几百KiB或者1MiB及以上。随机的小块读需要应用去做优化，以减少对hdd的随机寻道。
- 支持大块顺序写和小块随机写。小块随机写不会做针对性优化，效率可能会比较低。
- 重点支持场景：多客户端同时追加以支持多生产者多路归并的的应用场景。最小开销的同步机制是必要的，以方便后续多个消费者同时读取。
- 高带宽优先于低延时
## 接口
- 提供类posix接口，支持create、delete、open、close、read、write
- 额外支持低成本快照和多生产者原子追加功能
## 架构
![image](https://github.com/LoveYouMoreThanCode/tiny_trick/assets/141383156/940e8b45-98dc-45d0-b8fd-5f639c5e3524)
- 单master + 多chunkserver + 多client，用户态程序，部署形态比较灵活
- 文件被切分为chunk，每个chunk使用由master分配的一个64 bit唯一标识符。chunk以文件方式存在chunk server的本地磁盘。为了保证可靠性，chunk默认以3副本在chunkserver间复制。
- master维护所有文件系统元数据，包括namespace，acl，file到chunks的映射关系，chunk的位置信息。同时管理系统活动，比如chunk lease管理、孤儿chunk删除、chunk迁移。定期和chunkserver心跳，发送指令和收集状态。
- client以lib方式嵌入到用户程序，不提供标准posix api
- client和chunkserver均没有提供cache机制。client不需要的原因是，生产者和消费者大部分在不同位置；chunkserver不需要cache是因为本地文件系统已经存在cache了。
## single master
- IO路径不走master
- client会向master要chunk的位置信息，同时需要做一定时间的缓存
## chunk size
- 64MB。一个linux裸文件，依靠linux文件瘦分配来减少小chunk对空间的占用。
- 优点：1.减少和master交互 2.tcp链接可以较长时间复用，减少网络开销 3.元数据量少，master可以全缓存
- 大部分场景不存在热点问题。实际上确实遇到了热点问题：短期通过扩大副本数解决，未来长久解决措施希望允许通过其他客户端读取数据。
## metadata
- 三类数据：file和chunk的namespace、file到chunk的映射关系、chunk副本的位置信息
- 全内存存放，namespace&file to chunk mapping使用log持久化，chunk locations通过询问chunkserver获得
### in memory data structures
- 很快，操作很快。周期扫描高效且容易，周期扫描作用是收集垃圾chunk、故障复制、重均衡等
- 集群规模受限于master内存大小。大部分chunk都是满的，64MB一个chunk需要64bytes的meta。namespace平均每个文件小于64bytes，因为可以使用前缀压缩。
- 如果需要扩展，论文认为增加master的内存开销也不是很大。
### chunk locations
- 不持久化，只有在启动的时候轮询chunkserver获得
- master自身分配，自身可以位置最新的数据。
- 不用维护chunkserver的状态，比如join、leave、change names、fail、restart等等
### operation log
- 强复制到其他节点并保证sync后，才恢复客户端成功
- 缩短恢复时间，使用checkpoint技术，类似b+树结构可以直接映射到内存。
- 为了缩短checkpoint对元数据操作影响，使用切换的方式来实现。
## 一致性模型
### guarantees
- file namespace修改是原子的
- namespace的lock是原子的
- data更新
![image](https://github.com/LoveYouMoreThanCode/tiny_trick/assets/141383156/8c90f89d-accd-4d68-a213-822f88ae8354)

### Implications for Applications

## 系统交互
### 租约和修改顺序
- 60秒初始租期
- 通过心跳续约
- 修改顺序
  - client询问master获得chunk locations和primary chunk
  - master回复相关信息，client缓存住。只有primary不可达或者primary不再是主的时候才需要再次查询。
  - 将数据推送到所有replicas，不用管primary还是secondary
  - 一旦所有replicas都ack收到了数据，client向primary发起write数据操作。主节点会为修改分配连续的序列号
  - primary转发write到secondary replicas。每个secondary replicas执行由primary编号的顺序
  - secondary完成的数据写入后，回复primary
  - primary回复客户端。
  跨chunk的数据可能导致，数据行为位置。
### data flow
- 数据传输使用串行传递，充分利用每个机器的出入口带宽
- 转发data，就近转发（通过ip地址来评估距离）
### atomic append






