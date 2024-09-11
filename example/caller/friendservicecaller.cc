#include <mprpc/logger.h>
#include <mprpc/mprpcapplication.h>
#include <mprpc/mprpcchannel.h>
#include <mprpc/mprpccontroller.h>

#include <iostream>
#include <string>
#include <vector>

#include "friend.pb.h"

int main(int argc, char *argv[]) {
  MprpcApplication::Init(argc, argv);
  // 调用远程发布的rpc方法Login
  // Stub类有一个Channel基类指针成员变量
  // 传入重写了Channel CallMethod方法的派生类指针
  // Stub类的所有方法都通过Channel的CallMethod方法集中调用
  fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());

  // rpc方法的登录请求参数
  fixbug::GetFriendListRequest request;
  request.set_userid(999);

  // rpc方法的登录响应
  fixbug::GetFriendListResponse response;
  // 定义控制对象
  MprpcController controller;
  // 发起rpc方法的调用,同步的rpc调用过程
  stub.GetFriendList(&controller, &request, &response, nullptr);
  // 一次rpc调用完成，读取调用结果
  if (controller.Failed()) {
    LOG_ERROR("%s:%s:%d --- %s.", __FILE__, __FUNCTION__, __LINE__,
              controller.ErrorText().c_str());
  } else {
    if (0 == response.result().errcode()) {
      std::cout << "RPC GetFriendList response succeed!" << std::endl;
      int size = response.friend__size();
      for (int i = 0; i < size; ++i) {
        std::cout << "index:" << (i + 1) << " name:" << response.friend_(i)
                  << std::endl;
      }
    } else {
      LOG_ERROR("RPC login response error: %d %s.", response.result().errcode(),
                response.result().errmsg().c_str());
    }
  }
}
