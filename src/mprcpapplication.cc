#include <unistd.h>

#include <iostream>
#include <string>

#include "mprpcapplication.h"

namespace {
inline void ShowArgsHelp() {
  std::cout << "format: command -i <config>" << std::endl;
}
}  // namespace

void MprpcApplication::Init(int argc, char* argv[]) {
  if (argc < 2) {
    ShowArgsHelp();
  }
  int c = 0;
  std::string config_file;
  while (c = ::getopt(argc, argv, ":i:") != -1) {
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

  //开始加载配置文件
}

MprpcApplication& MprpcApplication::GetInstance() {
  static MprpcApplication app;
  return app;
}