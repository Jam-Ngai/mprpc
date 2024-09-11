#include <mprpc/logger.h>
#include <mprpc/mprpcapplication.h>
#include <mprpc/rpcprovider.h>

#include <iostream>
#include <string>
#include <vector>

#include "friend.pb.h"

class FriendService : public ::fixbug::FriendServiceRpc {
 public:
  // 做本地业务
  std::vector<std::string> GetFriendList(uint32_t userid) {
    std::cout << "Doing GetFriendList service." << std::endl;
    std::vector<std::string> friends;
    friends.emplace_back("Jack");
    friends.emplace_back("Bill");
    friends.emplace_back("Ryn");
    return friends;
  }
  // 重写基类方法
  void GetFriendList(::google::protobuf::RpcController* controller,
                     const ::fixbug::GetFriendListRequest* request,
                     ::fixbug::GetFriendListResponse* response,
                     ::google::protobuf::Closure* done) override {
    uint32_t userid = request->userid();
    std::vector<std::string> frinedslist = GetFriendList(userid);
    response->mutable_result()->set_errcode(0);
    response->mutable_result()->set_errmsg("");
    for (auto& e : frinedslist) {
      std::string* name = response->add_friend_();
      *name = e;
    }
    done->Run();
  }
};

int main(int argc, char* argv[]) {
  // 初始化RPC框架
  MprpcApplication::Init(argc, argv);

  // provider是一个rpc网络服务对象，把UserService对象发布到rpc节点上
  RpcProvider provider;
  provider.NotifyService(new FriendService());

  provider.Run();
  return 0;
}