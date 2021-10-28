#include "utf8string.hpp"

#define TESTFUNC0(f) str.f(); std::cout << str << std::endl;
#define TESTFUNC1(f, x) str.f(x); std::cout << str << std::endl;
#define TESTFUNC2(f, x, y) str.f(x, y); std::cout << str << str::endl;

int main() {
    utf8::String str { "test" };
    utf8::String::iterator i = str.begin();
    ++i; ++i; ++i;
    TESTFUNC1(erase, i);


    return 0;
}