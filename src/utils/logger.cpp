#include "utils/logger.hpp"
#include <ctime>
#include <fstream>
#include <print>
#include <sstream>

namespace {

constexpr std::string_view levelToString(LogLevel level) noexcept {
  switch (level) {
  case LogLevel::Debug:
    return "DEBUG";
  case LogLevel::Info:
    return "INFO";
  case LogLevel::Warning:
    return "WARNING";
  case LogLevel::Error:
    return "ERROR";
  case LogLevel::Critical:
    return "CRITICAL";
  }
  return "UNKNOWN";
}
} // namespace

namespace {

std::string currentTimestamp() {
  const std::time_t now = std::time(nullptr);

  std::tm local{};
  ::localtime_r(&now, &local);

  std::array<char, 32> buffer{};
  std::strftime(buffer.data(), buffer.size(), "%F %T", &local);
  return buffer.data();
}

} // namespace

Logger::Logger(const std::filesystem::path &filename)
    : m_logFile{filename, std::ios::app} {
  if (!m_logFile) {
    throw std::runtime_error{
        std::format("Error while opening the log file: {}", filename.string())};
  }
}

void Logger::log(LogLevel level, std::string_view message) {

  const std::string entry = std::format("[{}] {}: {}\n", currentTimestamp(),
                                        levelToString(level), message);

  std::print("{}", entry);

  m_logFile << entry;
  m_logFile.flush();
}
