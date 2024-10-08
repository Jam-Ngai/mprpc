#pragma once
#include <google/protobuf/service.h>

#include <string>

class MprpcController : public google::protobuf::RpcController {
 public:
  MprpcController();
  void Reset() override;
  bool Failed() const override;
  std::string ErrorText() const override;
  void SetFailed(const std::string& reason) override;

  // 未实现的功能
  void StartCancel() override;
  bool IsCanceled() const override;
  void NotifyOnCancel(google::protobuf::Closure* callback) override;

 private:
  bool failed_;             // 记录RPC方法执行的状态
  std::string error_text_;  // 记录错误信息
};
