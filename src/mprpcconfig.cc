#include "mprpcconfig.h"

#include <fstream>
#include <iostream>
#include <string>

#include "logger.h"

namespace {
void trim(std::string& str) {
  int beg = str.find_first_not_of(' ');
  int end = str.find_last_not_of(' ');
  str = str.substr(beg, end - beg + 1);
}
}  // namespace

void MprpcConfig::LoadConfigFile(const std::string& file) {
  std::ifstream fin(file, std::fstream::in);
  if (!fin.is_open()) {
    LOG_ERROR("%s:%s:%d --- %s is not exist!", __FILE__, __FUNCTION__, __LINE__,
              file.c_str());
    exit(EXIT_FAILURE);
  }

  std::string buf;
  while (std::getline(fin, buf)) {
    if (buf.empty()) continue;
    int idx = buf.find_first_not_of(" ");
    if (buf[idx] == '#') {
      continue;
    }
    idx = buf.find_first_of('=');
    std::string key(buf, 0, idx);
    std::string value(buf, idx + 1, buf.size());
    trim(key);
    trim(value);
    config_map_[key] = value;
  }
}

const std::string MprpcConfig::Find(const std::string& key) {
  auto it = config_map_.find(key);
  if (it != config_map_.end()) {
    return it->second;
  }
  return "";
}