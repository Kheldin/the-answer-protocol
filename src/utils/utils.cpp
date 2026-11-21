#include <algorithm>
#include <cctype>
#include <string_view>
bool has_whitespace(std::string_view s) {
  if (s.find_first_of(" \n\t\r\v\f") != std::string_view::npos) {
    return (true);
  }
  return (false);
}

bool has_only_alpha(std::string_view s) {
  if (s.empty()) {
    return (false);
  }
  return std::all_of(s.begin(), s.end(),
                     [](unsigned char c) { return std::isalpha(c) != 0; });
}
