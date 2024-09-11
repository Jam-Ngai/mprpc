#include "mprpcchannel.h"

#include <arpa/inet.h>
#include <errno.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <rpcheader.pb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "zookeeperutil.h"

void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                              google::protobuf::RpcController* controller,
                              const google::protobuf::Message* request,
                              google::protobuf::Message* response,
                              google::protobuf::Closure* done) {
  const google::protobuf::ServiceDescriptor* service_desc = method->service();
  std::string service_name = service_desc->name();
  std::string method_name = method->name();
  // 获取参数的序列化字符串长度，args_size
  std::string args_str;
  uint32_t args_size = 0;
  if (request->SerializeToString(&args_str)) {
    args_size = args_str.size();
  } else {
    controller->SetFailed("Serialize request failed!");
    return;
  }
  // 定义rpc的请求header
  mprpc::RpcHeader rpc_header;
  rpc_header.set_service_name(service_name);
  rpc_header.set_method_name(method_name);
  rpc_header.set_args_size(args_size);
  std::string rpc_header_str;
  uint32_t header_size;
  if (rpc_header.SerializeToString(&rpc_header_str)) {
    header_size = rpc_header_str.size();
  } else {
    controller->SetFailed("Serialize RPC header failed!");
    return;
  }

  // 组织待发送的rpc请求字符串
  std::string send_str;
  send_str.insert(0, std::string(reinterpret_cast<char*>(&header_size), 4));
  send_str += rpc_header_str;
  send_str += args_str;

#ifndef NDEBUG
  std::cout << "=================================================" << std::endl;
  std::cout << "header_size: " << header_size << std::endl;
  std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
  std::cout << "service_name: " << service_name << std::endl;
  std::cout << "method_name: " << method_name << std::endl;
  std::cout << "args_str: " << args_str << std::endl;
  std::cout << "=================================================" << std::endl;
#endif

  // 使用TCP编程，完成rpc方法的远程调用
  int clientfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == clientfd) {
    char text[1024] = {0};
    snprintf(text, sizeof text, "Create socket error: %d", errno);
    controller->SetFailed(text);
    return;
  }

  // std::string ip =
  // MprpcApplication::GetInstance().config().Find("rpcserverip"); uint16_t port
  // = atoi(
  //     MprpcApplication::GetInstance().config().Find("rpcserverport").c_str());
  //查询zookeeper上/service/method所在host信息
  ZkClient zkclient;
  zkclient.Start();
  std::string method_path = "/" + service_name + "/" + method_name;
  std::string host_data = zkclient.GetData(method_path.c_str());
  if (host_data == "") {
    controller->SetFailed(method_path + " is not exist!");
    return;
  }
  int idx = host_data.find(":");
  if (idx == -1) {
    controller->SetFailed(method_path + "is invalid!");
    return;
  }
  std::string ip = host_data.substr(0, idx);
  uint16_t port =
      atoi(host_data.substr(idx + 1, host_data.size() - idx).c_str());

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_aton(ip.c_str(), &addr.sin_addr);
  if (-1 == connect(clientfd, (sockaddr*)&addr, sizeof addr)) {
    char text[1024] = {0};
    snprintf(text, sizeof text, "Connect failed, error: %d", errno);
    close(clientfd);
    return;
  }
  if (-1 == send(clientfd, send_str.c_str(), send_str.size(), 0)) {
    char text[1024] = {0};
    snprintf(text, sizeof text, "Send failed, error: %d", errno);
    close(clientfd);
    return;
  }
  char buf[1024];
  int n = 0;
  if (-1 == (n = recv(clientfd, buf, 1024, 0))) {
    char text[1024] = {0};
    snprintf(text, sizeof text, "Recv failed, error: %d", errno);
    close(clientfd);
    return;
  }

  // 反序列化rpc调用的响应数据
  if (!response->ParseFromArray(buf, n)) {
    controller->SetFailed("Parse response failed!");
    close(clientfd);
    return;
  }
  close(clientfd);
}