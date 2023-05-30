#include "strings.hpp"

bool HasInStr(const std::string& str, const std::string& substr) {
  return str.find(substr) != std::string::npos;
}