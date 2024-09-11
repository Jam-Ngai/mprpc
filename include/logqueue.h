#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

// 异步日志队列
template <typename T>
class LogQueue {
 public:
  void push(const T& data);
  T pop();

 private:
  std::queue<T> queue_;
  std::mutex mtx_;
  std::condition_variable cond_var_;
};

//多个worker线程写入队列
template <typename T>
void LogQueue<T>::push(const T& data) {
  std::lock_guard<std::mutex> lock(mtx_);
  queue_.push(data);
  cond_var_.notify_one();
}

//1个线程读取队列写入日志文件
template <typename T>
T LogQueue<T>::pop() {
  std::unique_lock<std::mutex> lock(mtx_);
  cond_var_.wait(lock, [&]() { return !queue_.empty(); });
  T data = queue_.front();
  queue_.pop();
  return data;
}
