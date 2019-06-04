include(CheckCXXSourceCompiles)

set(CMAKE_REQUIRED_FLAGS "-std=c++17")
set(CMAKE_REQUIRED_LIBRARIES stdc++fs)

check_cxx_source_compiles("
#include <filesystem>
int main() {
    std::filesystem::path aPath {\"hello\"};
    aPath /= \"world\";
    return 0;
}
" WORKING_STD_FS)

set(CMAKE_REQUIRED_FLAGS)
set(CMAKE_REQUIRED_LIBRARIES)
