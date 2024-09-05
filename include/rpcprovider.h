#pragma once
#include <google/protobuf/service.h>

// 发布rpc服务的网络对象类
class RpcProvider {
 public:
  // 提供给外部发布RPC方法的接口
  void NotifyService(google::protobuf::Service *service);
  // 启动RPC服务节点，开始提供rpc远程网络调用服务
  void Run();
};