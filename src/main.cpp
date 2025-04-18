#include <iostream>
#include "ktest.hpp"

KTEST(hello_test) {
    const std::vector<std::string> vec;
    KASSERT_TRUE(vec.empty());
}

KTEST(hello_other_test) {
    KASSERT_EQ(5, 2 + 3);
}

int main() {
    ktest::runAllTests();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}