#include <iostream>
#include "include/thread_pool.h"

int add(int x, int y) {
    return x + y;
}

int main() {
    cc::ThreadPool tp(1);
    auto f1 = tp.Submit(add, 1, 3);
    auto f2 = tp.Submit(add, 3, 5);
    auto f3 = tp.Submit(add, 55, 3);
    f1.wait();
    f2.wait();
    f3.wait();
    std::cout << f1.get() << " " << f2.get() << " " << f3.get() << std::endl;
    return 0;
}