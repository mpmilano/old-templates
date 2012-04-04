#include "oxthread.h"
#include "assert_once.h"
#include <iostream>

int main(int, char**){

    using namespace std;

    string print = "foo";
    auto f = [&](){std::cout << print << std::endl; return true;};

for (int i = 0; i < 10; ++i){
assert_once(f());
assert_once(f());
}
}
