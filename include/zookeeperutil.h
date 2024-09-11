#pragma once

#include <semaphore.h>
#include <zookeeper/zookeeper.h>

#include <string>

class ZkClient {
 public:
  ZkClient();
  ~ZkClient();
  // 连接zkserver
  void Start();
  // 在zkserver上根据指定的path创建znode节点
  void Create(const char* path, const char* data, int datalen, int state = 0);
  // 根据path获取znode的值
  std::string GetData(const char* path);

 private:
  // zk客户端句柄
  zhandle_t* zhandle_;
};