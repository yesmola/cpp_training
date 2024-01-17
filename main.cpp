#include <iostream>
#include "include/thread_safe_with_lock/thread_safe_lookup_table.h"

int main() {
    cctest::TEST_thread_safe_lookup_table();
    std::cout << "====== Thread Safe Lookup Table Success ======" << std::endl;
    return 0;
}