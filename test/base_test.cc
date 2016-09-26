#include <iostream>

#include "base/timeutils.h"

int test_time()
{
    std::cout << fruite::SystemTimeNanos() << std::endl;
    return 0;
}
int main(int argc, char *argv[])
{
    test_time();
    return 0;
}
