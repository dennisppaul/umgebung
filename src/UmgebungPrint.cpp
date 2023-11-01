#include <iostream>
#include <sstream>

template<typename... Args>
void println(const Args &... args) {
    std::ostringstream os;
    (os << ... << args);
    std::cout << os.str() << std::endl;
}

template<typename... Args>
std::string to_string(const Args &... args) {
    std::ostringstream oss;
    (oss << ... << args);
    return oss.str();
}
