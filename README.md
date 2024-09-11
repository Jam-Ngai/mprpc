# mprpc
### 项目介绍

基于[Reactor高性能网络框架](https://github.com/Jam-Ngai/mymuduo.git)和Protubuf实现的分布式网络通信框架。

网络框架部分采用 epoll 实现了基于 Reactor 模式的事件循环系统，构建了一个高效的线程池管理机制，实现任务的动态调度，支持高并发的客户端连接处理，且完全基于C++11实现。采用Protobuf实现数据的序列化和反序列化，采用Zookeeper为分布式应用程序提供一致性协调服务，使用单例模式实现了一个异步日志管理类，项目通过cmake编译和构建。

### 用法

- 安装库

**_使用前需先安装[mymuduo](https://github.com/Jam-Ngai/mymuduo.git)、[prorobuf](https://github.com/protocolbuffers/protobuf.git)、和[zookeeper](https://zookeeper.apache.org/releases.html)。_**

```shell
git clone https://github.com/Jam-Ngai/mprpc.git
cd mprpc
mkdir build
cmake ..
make
cd ..
sudo sh install.sh
```

- 编译测试程序

```shell
cd example
mkdir build
cd build
cmake ..
make
cd ..
```
