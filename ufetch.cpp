#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <sys/utsname.h>
#include <unistd.h>
#include <limits.h>
#include <iomanip>

// ---------------- Utility ----------------
static inline std::string trim(const std::string &s) {
    const auto start = s.find_first_not_of(" \t");
    if (start == std::string::npos) return "";

    const auto end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

static inline std::string stripQuotes(std::string s) {
    if (!s.empty() && s.front() == '"') s.erase(0, 1);
    if (!s.empty() && s.back() == '"') s.pop_back();
    return s;
}

static inline bool startsWith(const std::string &s, const std::string &prefix) {
    return s.rfind(prefix, 0) == 0;
}

// ---------------- System Info Struct ----------------
struct SystemInfo {
    std::string user;
    std::string host;
    std::string os;
    std::string kernel;
    std::string cpu;
    std::string ram;
};

// ---------------- OS ----------------
std::string getOS() {
    std::ifstream f("/etc/os-release");
    if (!f.is_open()) return "Unknown OS";

    std::string line;
    while (std::getline(f, line)) {
        if (startsWith(line, "PRETTY_NAME=")) {
            auto pos = line.find('=');
            if (pos == std::string::npos) continue;

            return stripQuotes(line.substr(pos + 1));
        }
    }

    return "Unknown OS";
}

// ---------------- Kernel ----------------
std::string getKernel() {
    struct utsname buf;
    if (uname(&buf) == 0)
        return buf.release;

    return "Unknown Kernel";
}

// ---------------- User ----------------
std::string getUsername() {
    const char* user = getenv("USER");
    return user ? user : "unknown";
}

// ---------------- Host ----------------
std::string getHostname() {
    char hostname[HOST_NAME_MAX] = {0};
    if (gethostname(hostname, HOST_NAME_MAX) == 0)
        return hostname;

    return "unknown";
}

// ---------------- RAM (robust + auto scaling) ----------------
std::string formatMemory(long kb) {
    double value = kb;

    const char* unit = "KB";
    if (value > 1024 * 1024) {
        value /= (1024 * 1024);
        unit = "GB";
    } else if (value > 1024) {
        value /= 1024;
        unit = "MB";
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1)
        << value << " " << unit;

    return oss.str();
}

std::string getRAM() {
    std::ifstream f("/proc/meminfo");
    if (!f.is_open()) return "Unknown RAM";

    std::string line;
    long total = -1, available = -1;

    while (std::getline(f, line)) {
        std::istringstream iss(line);
        std::string key;
        long value;
        std::string unit;

        if (!(iss >> key >> value >> unit)) continue;

        if (key == "MemTotal:") total = value;
        else if (key == "MemAvailable:") available = value;
    }

    if (total < 0 || available < 0) return "Unknown RAM";

    long used = total - available;

    return formatMemory(used) + " / " + formatMemory(total);
}

// ---------------- CPU ----------------
std::string getCPU() {
    std::ifstream f("/proc/cpuinfo");
    if (!f.is_open()) return "Unknown CPU";

    std::string line;
    while (std::getline(f, line)) {
        if (startsWith(line, "model name")) {
            auto pos = line.find(':');
            if (pos == std::string::npos) continue;

            return trim(line.substr(pos + 1));
        }
    }

    return "Unknown CPU";
}

// ---------------- Build System Info ----------------
SystemInfo collectInfo() {
    SystemInfo info;

    info.user   = getUsername();
    info.host   = getHostname();
    info.os     = getOS();
    info.kernel = getKernel();
    info.cpu    = getCPU();
    info.ram    = getRAM();

    return info;
}

// ---------------- Output (pretty) ----------------
void printPretty(const SystemInfo& i) {
    std::cout << i.user << "@" << i.host << "\n";
    std::cout << "-----------------\n";
    std::cout << "OS:     " << i.os << "\n";
    std::cout << "Kernel: " << i.kernel << "\n";
    std::cout << "CPU:    " << i.cpu << "\n";
    std::cout << "RAM:    " << i.ram << "\n";
}

// ---------------- Output (JSON mode) ----------------
void printJSON(const SystemInfo& i) {
    std::cout << "{\n";
    std::cout << "  \"user\": \"" << i.user << "\",\n";
    std::cout << "  \"host\": \"" << i.host << "\",\n";
    std::cout << "  \"os\": \"" << i.os << "\",\n";
    std::cout << "  \"kernel\": \"" << i.kernel << "\",\n";
    std::cout << "  \"cpu\": \"" << i.cpu << "\",\n";
    std::cout << "  \"ram\": \"" << i.ram << "\"\n";
    std::cout << "}\n";
}

// ---------------- MAIN ----------------
int main(int argc, char** argv) {
    SystemInfo info = collectInfo();

    bool json = false;
    if (argc > 1 && std::string(argv[1]) == "--json") {
        json = true;
    }

    if (json)
        printJSON(info);
    else
        printPretty(info);

    return 0;
}
