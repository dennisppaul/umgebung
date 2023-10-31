#include <sstream>
#include <string>
#include <iostream>

template<typename... Args>
void println(const Args &... args) {
    std::ostringstream os;
    (os << ... << args);
    std::cout << os.str() << std::endl;
}