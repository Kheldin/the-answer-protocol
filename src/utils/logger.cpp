#include "utils/logger.hpp"
#include <array>
#include <ctime>
#include <format>
#include <print>
#include <string_view>
#include <unistd.h>

namespace {

constexpr std::string_view kReset = "\033[0m";
constexpr std::string_view kPurple = "\033[38;2;189;147;249m"; // #BD93F9
constexpr std::string_view kDim = "\033[38;2;98;114;164m";     // #6272A4
constexpr std::string_view levelToColor(LogLevel level) noexcept {
  switch (level) {
  case LogLevel::Debug:
    return "\033[90m"; // grey
  case LogLevel::Info:
    return "\033[32m"; // green
  case LogLevel::Warning:
    return "\033[33m"; // yellow
  case LogLevel::Error:
    return "\033[31m"; // red
  case LogLevel::Critical:
    return "\033[1;97;41m"; // white on red background
  }
  return "";
}

bool colorEnabled() {
  static const bool enabled = ::isatty(STDOUT_FILENO) != 0;
  return enabled;
}

} // namespace
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

  const std::string timestamp = currentTimestamp();
  const std::string_view name = levelToString(level);

  const std::string plain =
      std::format("[{}] {}: {}", timestamp, name, message);

  if (colorEnabled()) {
    std::print("{}[{}] {}{}{} {}{}", kDim, timestamp, levelToColor(level), name,
               kPurple, message, kReset);
  } else {
    std::print("{}", plain);
  }

  m_logFile << plain;
  m_logFile.flush();
}
