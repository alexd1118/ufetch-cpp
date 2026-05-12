#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <sys/utsname.h>
#include <unistd.h>
#include <limits.h>

std::string trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

std::string stripQuotes(std::string s) {
    if (!s.empty() && s.front() == '"') s.erase(0, 1);
    if (!s.empty() && s.back() == '"') s.pop_back();
    return s;
}

// ---------------- OS ----------------
std::string getOS() {
    std::ifstream f("/etc/os-release");
    if (!f.is_open()) {
        f.open("/usr/lib/os-release"); // fallback
        if (!f.is_open()) return "Unknown";
    }

    std::string line;
    while (std::getline(f, line)) {
        if (line.rfind("PRETTY_NAME=", 0) == 0) {
            std::string value = line.substr(line.find('=') + 1);
            return stripQuotes(value);
        }
    }
    return "Unknown";
}

// ---------------- Kernel ----------------
std::string getKernel() {
    struct utsname buffer;
    if (uname(&buffer) == 0)
        return buffer.release;
    return "Unknown";
}

// ---------------- User ----------------
std::string getUsername() {
    const char* user = getenv("USER");
    return user ? user : "unknown";
}

// ---------------- Host ----------------
std::string getHostname() {
    char hostname[HOST_NAME_MAX] = {0};
    gethostname(hostname, HOST_NAME_MAX);
    return hostname;
}

// ---------------- RAM ----------------
std::string getRAM() {
    std::ifstream f("/proc/meminfo");
    if (!f.is_open()) return "Unknown";

    std::string line;
    long total = 0, available = 0;

    while (std::getline(f, line)) {
        std::istringstream iss(line);
        std::string key;
        long value;
        std::string unit;

        iss >> key >> value >> unit;

        if (key == "MemTotal:") total = value;
        if (key == "MemAvailable:") available = value;
    }

    if (total == 0) return "Unknown";

    long used = total - available;

    return std::to_string(used / 1024) + "MB / " +
           std::to_string(total / 1024) + "MB";
}

// ---------------- CPU (basic but portable) ----------------
std::string getCPU() {
    std::ifstream f("/proc/cpuinfo");
    if (!f.is_open()) return "Unknown CPU";

    std::string line;
    while (std::getline(f, line)) {
        if (line.rfind("model name", 0) == 0) {
            return line.substr(line.find(':') + 2);
        }
    }
    return "Unknown CPU";
}

// ---------------- MAIN ----------------
int main() {
    std::cout << getUsername() << "@" << getHostname() << "\n";
    std::cout << "-----------------\n";
    std::cout << "OS:     " << getOS() << "\n";
    std::cout << "Kernel: " << getKernel() << "\n";
    std::cout << "CPU:    " << getCPU() << "\n";
    std::cout << "RAM:    " << getRAM() << "\n\n";

    return 0;
}
