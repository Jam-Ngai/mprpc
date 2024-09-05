#include <iostream>
#include <string>

#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

// RPC服务提供方
class UserService : public fixbug::UserServiceRpc {
 public:
  bool Login(const std::string& name, const std::string& pwd) {
    std::cout << "doing local service: Login" << std::endl;
    std::cout << "name: " << name << "pwd: " << pwd << std::endl;
    return true;
  }

  // 重写UserServiceRpc基类的虚函数
  void Login(::google::protobuf::RpcController* controller,
             const ::fixbug::LoginRequest* request,
             ::fixbug::LoginResponse* response,
             ::google::protobuf::Closure* done) override {
    // RPC框架给业务上报请求参数LogRequest，应用获取参数做本地业务
    std::string name = request->name();
    std::string password = request->passwprd();
    // 做本地业务
    bool login_result = Login(name, password);
    // 把响应写入LogResponse，通过框架发送
    fixbug::ResultCode* code = response->mutable_result();
    code->set_errcode(0);
    code->set_errms("");
    response->set_succeed(login_result);
    // 执行回调，执行响应对象数据的序列化和网络发送
    done->Run();
  }
};

int main(int argc, char* argv[]) {
  //初始化RPC框架
  MprpcApplication::Init(argc,argv);

  //provider是一个rpc网络服务对象，把UserService对象发布到rpc节点上
  RpcProvider provider;
  provider.NotifyService(new UserService());

  provider.Run();
  return 0;
}