#include "logger.h"

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <thread>

Logger::Logger() {
  std::thread write_log([&]() {
    for (;;) {
      time_t now = time(nullptr);
      tm* now_tm = localtime(&now);
      char file_name[128];
      if (access("./logs", F_OK) != 0) {
        if (mkdir("./logs", 0755) != 0) {
          perror("Create log directory failed.");
          exit(EXIT_FAILURE);
        }
      }
      snprintf(file_name, sizeof file_name, "./logs/%d-%d-%d-log.txt",
               now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday);
      FILE* pfile = fopen(file_name, "a+");
      if (!pfile) {
        std::cout << "Logger file: " << file_name << " open failed!"
                  << std::endl;
        exit(EXIT_FAILURE);
      }
      std::string msg = log_queue_.pop();
      char time_buf[128] = {0};
      snprintf(time_buf, sizeof time_buf, "%d:%d:%d --- ", now_tm->tm_hour,
               now_tm->tm_min, now_tm->tm_sec);
      msg.insert(0, time_buf);
      msg.push_back('\n');
      fputs(msg.c_str(), pfile);
      fclose(pfile);
    }
  });
  // 设置分离线程，守护线程
  write_log.detach();
}

void Logger::Log(LogLevel level, const std::string& str) {
  switch (level) {
    case INFO:
      log_queue_.push("[INFO]" + str);
      break;
    case ERROR:
      log_queue_.push("[ERROR]" + str);
      break;
  }
}

Logger& Logger::GetInstance() {
  static Logger logger;
  return logger;
}
