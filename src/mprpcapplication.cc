#include "mprpcapplication.h"

#include <unistd.h>

#include <iostream>
#include <string>

#include "logger.h"

namespace {
inline void ShowArgsHelp() {
  std::cout << "format: command -i <config>" << std::endl;
}
}  // namespace

MprpcConfig MprpcApplication::config_;

void MprpcApplication::Init(int argc, char* argv[]) {
  if (argc < 2) {
    ShowArgsHelp();
    exit(EXIT_FAILURE);
  }
  int c = 0;
  std::string config_file;
  while ((c = ::getopt(argc, argv, ":i:")) != -1) {
    switch (c) {
      case 'i':
        config_file = optarg;
        break;
      case '?':
        ShowArgsHelp();
        exit(EXIT_FAILURE);
      case ':':
        ShowArgsHelp();
        exit(EXIT_FAILURE);
      default:
        break;
    }
  }

  // 加载配置文件
  config_.LoadConfigFile(config_file);
  LOG_INFO(
      "rpcserverip: %s, rpcserverport: %s, zookeeperip: %s, zookeeperport: %s.",
      config_.Find("rpcserverip").c_str(), config_.Find("rpcserverport").c_str(),
      config_.Find("zookeeperip").c_str(), config_.Find("zookeeperport").c_str());
}

MprpcApplication& MprpcApplication::GetInstance() {
  static MprpcApplication app;
  return app;
}

MprpcConfig& MprpcApplication::config() const { return config_; }