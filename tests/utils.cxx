#include <cassert>
#include <filesystem>

#include "utils.hxx"


void run_tests() {
    assert(std::filesystem::longest_common_basepath({}).string() == "");
    std::filesystem::path p1 = "/a/b/c";
    assert(std::filesystem::longest_common_basepath({p1}) == p1);
    assert(std::filesystem::longest_common_basepath({"/a/b/c", "/a/b/d"}) == "/a/b");
    assert(std::filesystem::longest_common_basepath({"/a/bc", "/a/b"}) == "/a");
    assert(std::filesystem::longest_common_basepath({"/a", "/b"}) == "/");
}

int main() {
    try {
        run_tests();
    } catch (...) {
        return 1;
    }
    return 0;
}