#pragma once
#include "mprpcconfig.h"

// rpc框架类
class MprpcApplication {
 public:
  static void Init(int argc, char *argv[]);
  static MprpcApplication &GetInstance();
  MprpcConfig& config() const;

 private:
  MprpcApplication() {};
  MprpcApplication(const MprpcApplication &) = delete;
  MprpcApplication(MprpcApplication &&) = delete;
  MprpcApplication &operator=(const MprpcApplication &) = delete;

  static MprpcConfig config_;
};