//
// Created by cyan on 4/21/25.
//

#include "ktest.hpp"

KTEST(hello_test) {
    const std::vector<std::string> vec;
    KASSERT_TRUE(vec.empty());
}

KTEST(hello_other_test) {
    KASSERT_EQ(5, 2 + 3);
}
