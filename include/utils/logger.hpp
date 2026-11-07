#pragma once

#include <filesystem>
#include <format>
#include <fstream>
#include <string_view>
enum class LogLevel { Debug, Info, Warning, Error, Critical };

class Logger {
public:
  explicit Logger(const std::filesystem::path &filename);
  ~Logger() { m_logFile.close(); }
  void log(LogLevel level, std::string_view message);
  template <typename... Args>
  void log(LogLevel level, std::format_string<Args...> fmt, Args &&...args) {
    log(level, std::format(fmt, std::forward<Args>(args)...));
  }

private:
  std::ofstream m_logFile;
};
