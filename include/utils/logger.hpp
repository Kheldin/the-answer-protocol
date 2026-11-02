#pragma once

#include <filesystem>
#include <fstream>
#include <string_view>

enum class LogLevel { Debug, Info, Warning, Error, Critical };

class Logger {
public:
  explicit Logger(const std::filesystem::path &filename);
  ~Logger() { m_logFile.close(); }
  void log(LogLevel level, std::string_view message);

private:
  std::ofstream m_logFile;
};
