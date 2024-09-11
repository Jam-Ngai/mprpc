#include <mprpc/mprpcapplication.h>
#include <mprpc/mprpcchannel.h>

#include <iostream>

#include "user.pb.h"

int main(int argc, char *argv[]) {
  MprpcApplication::Init(argc, argv);
  // 调用远程发布的rpc方法Login
  // Stub类有一个Channel基类指针成员变量
  // 传入重写了Channel CallMethod方法的派生类指针
  // Stub类的所有方法都通过Channel的CallMethod方法集中调用
  fixbug::UserServiceRpc_Stub stub(new MprpcChannel());

  // rpc方法的登录请求参数
  fixbug::LoginRequest login_request;
  login_request.set_name("Bill");
  login_request.set_passwprd("foo");

  // rpc方法的登录响应
  fixbug::LoginResponse login_response;
  // 发起rpc方法的调用,同步的rpc调用过程
  stub.Login(nullptr, &login_request, &login_response, nullptr);
  // 一次rpc调用完成，读取调用结果
  if (!login_response.succeed()) {
    std::cout << "RPC login response error: "
              << login_response.result().errcode() << " "
              << login_response.result().errmsg() << std::endl;
  } else {
    std::cout << "RPC login response succeed: " << login_response.succeed()
              << std::endl;
  }

  // 注册请求参数
  fixbug::RegisterRequest register_request;
  register_request.set_id(2000);
  register_request.set_name("Ryn");
  register_request.set_password("bar");

  // 注册响应
  fixbug::RegisterResponse register_response;
  stub.Register(nullptr, &register_request, &register_response, nullptr);
  if (!login_response.succeed()) {
    std::cout << "RPC register response error: "
              << register_response.result().errcode() << " "
              << register_response.result().errmsg() << std::endl;
  } else {
    std::cout << "RPC register response succeed: "
              << register_response.succeed() << std::endl;
  }
}