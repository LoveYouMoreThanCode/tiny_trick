# 1 Introduction
- Phase one of Pangu 2.0: embracing SSD and RDMA by file system refactoring and user-space storage operating system.
- Phase two of Pangu 2.0: adapting to a performanceoriented business model with infrastructure updates and breaking through network/memory/CPU bottlenecks.
# 2 Background
## 2.1 Overview of Pangu
![image](https://github.com/LoveYouMoreThanCode/tiny_trick/assets/141383156/8ddae727-1a2a-452c-a9d6-a9ef47f7ed13)
## 2.2 Design Goals of Pangu 2.0
- Low latency：100us，P999 ms-level
- High throughput
- Unified high-performance support to all services: EBS OSS database
## 2.3 Related Work
# 3 Phase One: Embracing SSD and RDMA
## 3.1 Append-Only File System
### 3.1.1 Unified, Append-Only Persistence Layer
![image](https://github.com/LoveYouMoreThanCode/tiny_trick/assets/141383156/13017b24-cc9a-4515-8c47-78f72fdcf822)
### 3.1.2 Heavyweight Client
### 3.1.3 Append-only Chunk Management
![image](https://github.com/LoveYouMoreThanCode/tiny_trick/assets/141383156/5e6b27a3-5e76-432d-bfc0-baf2bc4f6012)
### 3.1.4 Metadata Operation Optimization
- Parallel metadata processing.
- Big chunks with variable lengths.
- Caching chunk information at clients
- Chunk information request batching
- Speculative chunk information prefetching
- Data piggybacking to reduce one RTT: write with create
## 3.2 Chunkserver USSOS
### 3.2.1 User-Level Memory Management
### 3.2.2 User-Space Scheduling Mechanism
- Preventing a task from blocking the subsequent ones
- Priority scheduling to guarantee high QoS.
- Polling and event-driven switching (NAPI).
### 3.2.3 Append-Only USSFS
![image](https://github.com/LoveYouMoreThanCode/tiny_trick/assets/141383156/82e91b4e-ed1e-4e2e-96a6-919c15e71fc5)
## 3.3 High Performance SLA Guarantee
- Non-stop write
- Backup read
- Blacklisting: 两级，确定和非确定。区分tcp/rdma。
## 3.4 Evaluations
# 4 Phase Two: Adapting to PerformanceOriented Business Model
## 4.1 Network Bottleneck
### 4.1.1 Bandwidth Expansion
### 4.1.2 Traffic Optimization
- Use EC to replace 3-replica
- Compressing FlatLogFile.
- Dynamic bandwidth allocation between front-end and background traffic
## 4.2 Memory Bottleneck
### 4.2.1 Adding Small-Capacity DRAMs
### 4.2.2 Shifting Background Traffic From TCP to RDMA
### 4.2.3 Remote Direct Cache Access
## 4.3 CPU Bottleneck
### 4.3.1 Hybrid RPCs
### 4.3.2 Supporting Hyper-Threading Using CPU Wait
### 4.3.3 Hardware and Software Co-design















