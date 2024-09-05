#pragma once
#include <string>
#include <unordered_map>

// 解析配置文件
// rpc_service_ip rpc_service_port zookeeper_ip zookeeper_port
class MprpcConfig {
 public:
  // 加载配置文件
  void LoadConfigFile(const std::string& file);
  // 查找配置信息
  const std::string& Find(const std::string& key);

 private:
  std::unordered_map<std::string, std::string> configmap_;
};