#include <iostream>
#include "include/thread_safe_with_lock/thread_safe_lookup_table.h"
#include "include/memory/shared_ptr.h"

int main() {
    cctest::TEST_thread_safe_lookup_table();
    cctest::TEST_SharedPtr();
    return 0;
}