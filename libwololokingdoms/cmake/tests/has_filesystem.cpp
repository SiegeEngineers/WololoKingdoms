#include <filesystem>

namespace fs = std::filesystem;

int main() {
  fs::path aPath {"hello"};
  aPath /= "world";

  return 0;
}
