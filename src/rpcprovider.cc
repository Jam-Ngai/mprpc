#include "rpcprovider.h"

#include <mymuduo/InetAddress.h>
#include <mymuduo/TcpConnection.h>
#include <mymuduo/TcpServer.h>
#include <rpcheader.pb.h>

#include <functional>
#include <iostream>
#include <string>

#include "logger.h"
#include "mprpcapplication.h"
#include "zookeeperutil.h"

void RpcProvider::NotifyService(google::protobuf::Service *service) {
  ServiceInfo service_info;
  // 获取服务对象的描述信息
  const google::protobuf::ServiceDescriptor *service_desc_ptr_ =
      service->GetDescriptor();
  // 获取服务的名字
  std::string servive_name = service_desc_ptr_->name();
  // 获取服务对象service的方法数量
  int method_cnt = service_desc_ptr_->method_count();
  LOG_INFO("Service name: %s.", servive_name.c_str());
  for (int i = 0; i < method_cnt; ++i) {
    // 获取服务对象指定下标的服务方法抽象描述
    const google::protobuf::MethodDescriptor *method_ptr =
        service_desc_ptr_->method(i);
    std::string method_name = method_ptr->name();
    service_info.method_map_[method_name] = method_ptr;
    LOG_INFO("Method name: %s.", method_name.c_str());
  }
  service_info.service_ = service;
  service_map_[servive_name] = service_info;
}

void RpcProvider::Run() {
  std::string ip = MprpcApplication::GetInstance().config().Find("rpcserverip");
  uint16_t port = atoi(
      MprpcApplication::GetInstance().config().Find("rpcserverport").c_str());

  mymuduo::InetAddress addr(port, ip);
  // 创建tcpserver对象
  mymuduo::TcpServer server(&eventloop_, addr, "RpcProvider");
  // 设置线程数量
  server.SetThreadNum(4);
  // 绑定连接回调和消息读写回调
  // 新连接的事件
  server.SetConnectionCallback(
      std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
  // 已连接用户的读写事件
  server.SetMessageCallback(
      std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  LOG_INFO("RPC provider service start at ip: %s, port: %d", ip.c_str(), port);

  // 把当前rpc节点上要发布的服务全部注册到zk上，让rpc client可以从zk上发现服务
  ZkClient zkclient;
  zkclient.Start();
  // service_name 为永久性节点，method_name为临时性节点
  for (auto &serviec : service_map_) {
    // 添加 /service_name节点
    std::string service_path = "/" + serviec.first;
    zkclient.Create(service_path.c_str(), nullptr, 0);
    for (auto &method : serviec.second.method_map_) {
      std::string method_path = service_path + "/" + method.first;
      char method_data[128] = {0};
      snprintf(method_data, sizeof method_data, "%s:%d", ip.c_str(), port);
      // ZOO_EPHEMERAL表示临时性节点
      zkclient.Create(method_path.c_str(), method_data, strlen(method_data),
                      ZOO_EPHEMERAL);
    }
  }

  server.Start();
  eventloop_.Loop();
}

void RpcProvider::OnConnection(const mymuduo::TcpConnectionPtr &conn) {
  // 和RPC client连接断开
  if (conn->Disconnected()) {
    conn->Shutdown();
  }
}

// 如果远程有一个rpc服务的调用请求，OnMessage就会响应
void RpcProvider::OnMessage(const mymuduo::TcpConnectionPtr &conn,
                            mymuduo::Buffer *buffer, mymuduo::Timestamp) {
  // 网络上接受的远程rpc调用的字符流
  // 包括 服务名 方法名 参数
  //|---数据头长度---||----------------数据头--------------||---参数---|
  //|---head_size---||service_name method_name args_size||---args---|
  //|----4 byte-----||service_name method_name args_size||---args---|
  std::string recv_buf = buffer->RetrieveAllAsString();
  // 从字符流中读取前4个字节
  uint32_t header_size = 0;
  recv_buf.copy(reinterpret_cast<char *>(&header_size), 4);
  // 根据header_size读取数据头的原始字符流
  std::string rpc_header_str = recv_buf.substr(4, header_size);
  // 反序列数据，得到rpc的详细信息
  mprpc::RpcHeader rpc_header;
  std::string service_name;
  std::string method_name;
  uint32_t args_size;
  if (rpc_header.ParseFromString(rpc_header_str)) {
    // 反序列化成功
    service_name = rpc_header.service_name();
    method_name = rpc_header.method_name();
    args_size = rpc_header.args_size();
  } else {
    LOG_ERROR("%s:%s:%d --- rpc_header_str: %s parse error!", __FILE__,
              __FUNCTION__, __LINE__, rpc_header_str.c_str());
    return;
  }

  // 获取rpc方法的参数
  std::string args_str = recv_buf.substr(4 + header_size, args_size);

#ifndef NDEBUG
  std::cout << "=================================================" << std::endl;
  std::cout << "header_size: " << header_size << std::endl;
  std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
  std::cout << "service_name: " << service_name << std::endl;
  std::cout << "method_name: " << method_name << std::endl;
  std::cout << "args_str: " << args_str << std::endl;
  std::cout << "=================================================" << std::endl;
#endif

  // 获取service对象
  auto service_it = service_map_.find(service_name);
  if (service_it == service_map_.end()) {
    LOG_ERROR("%s:%s:%d --- %s is not exit!", __FILE__, __FUNCTION__, __LINE__,
              service_name.c_str());
    return;
  }
  google::protobuf::Service *service = service_it->second.service_;

  // 获取method对象
  auto method_it = service_it->second.method_map_.find(method_name);
  if (method_it == service_it->second.method_map_.end()) {
    LOG_ERROR("%s:%s:%d --- %s/%s is not exit!", __FILE__, __FUNCTION__,
              __LINE__, service_name.c_str(), method_name.c_str());
    return;
  }
  const google::protobuf::MethodDescriptor *method = method_it->second;

  // 生成rpc方法调用的请求request对象和响应response对象
  google::protobuf::Message *request =
      service->GetRequestPrototype(method).New();
  google::protobuf::Message *response =
      service->GetResponsePrototype(method).New();

  // 反序列化rpc方法参数
  if (!request->ParseFromString(args_str)) {
    LOG_ERROR("%s:%s:%d --- request parse error, args: %s.", __FILE__,
              __FUNCTION__, __LINE__, args_str.c_str());
    return;
  }

  // 给下面的method方法，绑定一个Closure回调函数
  // NewCallback返回一个Closure的指针,该指针世纪只想Closure的派生类对象
  // 该对象重写了Closure的Run函数来调用构造该派生类对象时传入的method
  google::protobuf::Closure *done =
      google::protobuf::NewCallback<RpcProvider,
                                    const mymuduo::TcpConnectionPtr &,
                                    google::protobuf::Message *>(
          this, &RpcProvider::SendRpcResponse, conn, response);

  // 在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
  // service指针利用了多态，指向重写了相应method的具体的服务类
  // CallMenthd通过传入的method指针，判断调用哪个方法
  service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::SendRpcResponse(const mymuduo::TcpConnectionPtr &conn,
                                  google::protobuf::Message *response) {
  std::string response_str;
  if (response->SerializeToString(&response_str)) {
    // 序列化成功后，通过网络把rpc方法执行的结果发送给rpc的调用方
    conn->Send(response_str);
  } else {
    LOG_ERROR("%s:%s:%d --- Serialize response_str error!", __FILE__,
              __FUNCTION__, __LINE__);
  }
  // 模拟http的短链接服务，有rpcprovider主动断开连接
  conn->Shutdown();
}
