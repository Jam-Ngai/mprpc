#pragma once
#include "mprpcconfig.h"

// rpc框架类
class MprpcApplication {
 public:
  static void Init(int argc, char *argv[]);
  static MprpcApplication &GetInstance();

 private:
  MprpcApplication() {};
  MprpcApplication(const MprpcApplication &) = delete;
  MprpcApplication(MprpcApplication &&) = delete;
  MprpcApplication &operator=(const MprpcApplication &) = delete;

  MprpcConfig config_;
};