#include <fstream>
#include <iostream>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS_SYSTEM
#elif defined(__APPLE__)
#define MACOS_SYSTEM
#elif defined(__linux__)
#define LINUX_SYSTEM
#endif

bool isRaspberryPi() {
    std::ifstream file("/proc/device-tree/model");
    std::string line;
    if (file.is_open()) {
        getline(file, line);
        file.close();
        return line.find("Raspberry Pi") != std::string::npos;
    }
    return false;
}

int main() {
    #ifdef WINDOWS_SYSTEM
    std::cout << "Running on Windows." << std::endl;
    return 1;
    #elif defined(MACOS_SYSTEM)
    std::cout << "Running on macOS." << std::endl;
    return 2;
    #elif defined(LINUX_SYSTEM)
    if (isRaspberryPi()) {
        std::cout << "Running on Raspberry Pi." << std::endl;
        return 3; // Raspberry Pi
    } else {
        std::cout << "Running on Linux (non-Raspberry Pi)." << std::endl;
        return 4; // Other Linux
    }
    #else
    std::cout << "Running on an unknown system." << std::endl;
    return 0; // Unknown system
    #endif
}
