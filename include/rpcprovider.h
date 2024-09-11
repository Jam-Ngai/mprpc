#pragma once

#include <google/protobuf/descriptor.h>
#include <google/protobuf/service.h>
#include <mymuduo/Callbacks.h>
#include <mymuduo/EventLoop.h>

#include <memory>
#include <unordered_map>

// 发布rpc服务的网络对象类
class RpcProvider {
 public:
  // 提供给外部发布RPC方法的接口
  void NotifyService(google::protobuf::Service *service);
  // 启动RPC服务节点，开始提供rpc远程网络调用服务
  void Run();

 private:
  void OnConnection(const mymuduo::TcpConnectionPtr &);
  void OnMessage(const mymuduo::TcpConnectionPtr &, mymuduo::Buffer *,
                 mymuduo::Timestamp);

  // 回调操作，用于 序列化rpc的响应和网络发送
  void SendRpcResponse(const mymuduo::TcpConnectionPtr &,
                       google::protobuf::Message *);
  // 服务类型信息
  class ServiceInfo {
   public:
    // 服务对象
    google::protobuf::Service *service_;
    // 服务方法
    std::unordered_map<std::string, const google::protobuf::MethodDescriptor *>
        method_map_;
  };
  // 存储注册成功的服务对象和其服务方法的所有信息
  std::unordered_map<std::string, ServiceInfo> service_map_;
  mymuduo::EventLoop eventloop_;
};