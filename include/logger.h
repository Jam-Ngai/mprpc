#pragma once
#include <stdio.h>

#include <string>

#include "logqueue.h"

enum LogLevel {
  INFO,   // 普通信息
  ERROR,  // 错误信息
};

class Logger {
 public:
  void Log(LogLevel level, const std::string& str);
  static Logger& GetInstance();

 private:
  Logger();
  Logger(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(const Logger&) = delete;
  // 记录日志级别
  LogQueue<std::string> log_queue_;
};

#define LOG_INFO(logmsgformat, ...)                         \
  do {                                                      \
    Logger& logger = Logger::GetInstance();                 \
    char buf[1024] = {0};                                   \
    snprintf(buf, sizeof buf, logmsgformat, ##__VA_ARGS__); \
    logger.Log(INFO, buf);                                  \
  } while (0);

#define LOG_ERROR(logmsgformat, ...)                        \
  do {                                                      \
    Logger& logger = Logger::GetInstance();                 \
    char buf[1024] = {0};                                   \
    snprintf(buf, sizeof buf, logmsgformat, ##__VA_ARGS__); \
    logger.Log(ERROR, buf);                                 \
  } while (0);
