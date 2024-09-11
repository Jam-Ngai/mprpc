#include "zookeeperutil.h"

#include <iostream>

#include "logger.h"
#include "mprpcapplication.h"

namespace {
// 全局观察器
// zkserver给zkclient的通知
inline void global_watcher(zhandle_t *zh, int type, int state, const char *path,
                           void *watcherCtx) {
  if (type == ZOO_SESSION_EVENT) {
    if (state == ZOO_CONNECTED_STATE) {
      // 获取绑定的信号量
      sem_t *sem = (sem_t *)zoo_get_context(zh);
      sem_post(sem);
    }
  }
}
}  // namespace

ZkClient::ZkClient() : zhandle_(nullptr) {};

ZkClient::~ZkClient() {
  if (zhandle_ != nullptr) {
    // 关闭句柄，释放资源
    zookeeper_close(zhandle_);
  }
}

void ZkClient::Start() {
  std::string host =
      MprpcApplication::GetInstance().config().Find("zookeeperip");
  std::string port =
      MprpcApplication::GetInstance().config().Find("zookeeperport");
  std::string host_port = host + ":" + port;

  // zookeeper_mt多线程版本提供三个线程
  // API调用线程 网络IO线程 watcher回调线程
  // 异步连接
  zhandle_ = zookeeper_init(host_port.c_str(), global_watcher, 30000, nullptr,
                            nullptr, 0);
  // 资源分配失败，并不代表连接失败
  if (nullptr == zhandle_) {
    LOG_ERROR("%s:%s:%d --- Zookeeper initialize failed!", __FILE__,
              __FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  sem_t sem;
  sem_init(&sem, 0, 0);
  // 把信号量绑定到句柄
  zoo_set_context(zhandle_, &sem);
  sem_wait(&sem);
  LOG_INFO("Zookeeper initialize succeed!");
}

void ZkClient::Create(const char *path, const char *data, int datalen,
                      int state) {
  char path_buf[128] = {0};
  // 判断path表示的znode是否存在
  int flag = zoo_exists(zhandle_, path, 0, nullptr);
  if (ZNONODE == flag) {
    // 创建path指定的节点
    flag = zoo_create(zhandle_, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE,
                      state, path_buf, sizeof path_buf);
    if (ZOK == flag) {
      LOG_INFO("Zookeeper initialize succeed, path: %s.", path_buf);
    } else {
      LOG_ERROR("%s:%s:%d --- Create znode failed, path: %s, flag: %d",
                __FILE__, __FUNCTION__, __LINE__, path_buf, flag);
      exit(EXIT_FAILURE);
    }
  }
}

std::string ZkClient::GetData(const char *path) {
  char buf[64];
  int buf_len = sizeof buf;
  int flag = zoo_get(zhandle_, path, 0, buf, &buf_len, nullptr);
  if (ZOK == flag) {
    return buf;
  } else {
    LOG_ERROR("%s:%s:%d --- Get znode failed, path: %s.", __FILE__,
              __FUNCTION__, __LINE__, path);
    return "";
  }
}