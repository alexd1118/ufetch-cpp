#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <sys/utsname.h>

// ---------------- ASCII LOGOS ----------------

std::vector<std::string> arch = {
"       /\\        ",
"      /  \\       ",
"     /\\   \\      ",
"    /      \\     ",
"   /   ,,   \\    ",
"  /   |  |   \\   ",
" /_-''    ''-_\\  "
};

std::vector<std::string> ubuntu = {
"            _",
"        ---(_)",
"    _/  _/  _/ ",
"   / _/  _/    ",
"  /_/  _/      ",
"  (_)_/        "
};

std::vector<std::string> fedora = {
"      _____",
"     /  __ \\",
"    /  /  \\ \\",
"   |  |    | |",
"    \\  \\__/ /",
"     \\_____/ "
};

std::vector<std::string> mint = {
"      ______________",
"     |   _   _     |",
"     |  | | | |    |",
"     |  |_| |_|    |",
"     |_____________|"
};

std::vector<std::string> fallback = {
"    .--.",
"   |o_o |",
"   |:_/ |",
"  //   \\ \\",
" (|     | )",
"/'\\_   _/`\\",
"\\___)=(___/"
};

// ---------------- OS DETECTION ----------------

std::string getOSID() {
    std::ifstream f("/etc/os-release");
    std::string line;

    while (std::getline(f, line)) {
        if (line.rfind("ID=", 0) == 0) {
            return line.substr(3);
        }
    }
    return "linux";
}

std::vector<std::string> getLogo() {
    std::string id = getOSID();

    if (id.find("arch") != std::string::npos) return arch;
    if (id.find("ubuntu") != std::string::npos) return ubuntu;
    if (id.find("fedora") != std::string::npos) return fedora;
    if (id.find("mint") != std::string::npos) return mint;

    return fallback;
}

// ---------------- SYSTEM INFO ----------------

std::string getUser() {
    const char* u = getenv("USER");
    return u ? u : "user";
}

std::string getHost() {
    char buf[256] = {0};
    gethostname(buf, sizeof(buf));
    return buf;
}

std::string getKernel() {
    struct utsname u;
    if (uname(&u) == 0) return u.release;
    return "unknown";
}

std::string getOSPretty() {
    std::ifstream f("/etc/os-release");
    std::string line;

    while (std::getline(f, line)) {
        if (line.rfind("PRETTY_NAME=", 0) == 0) {
            return line.substr(line.find('=') + 1);
        }
    }
    return "Linux";
}

std::string getRAM() {
    std::ifstream f("/proc/meminfo");
    std::string line;

    long total = 0, avail = 0;

    while (std::getline(f, line)) {
        if (line.rfind("MemTotal:", 0) == 0)
            total = std::stol(line.substr(10));

        if (line.rfind("MemAvailable:", 0) == 0)
            avail = std::stol(line.substr(14));
    }

    long used = total - avail;

    return std::to_string(used / 1024) + "MB / " +
           std::to_string(total / 1024) + "MB";
}

// ---------------- PRINT ----------------

void print() {
    auto logo = getLogo();

    std::vector<std::string> info = {
        getUser() + "@" + getHost(),
        "OS:     " + getOSPretty(),
        "Kernel: " + getKernel(),
        "RAM:    " + getRAM()
    };

    int lines = std::max(logo.size(), info.size());

    for (int i = 0; i < lines; i++) {

        if (i < (int)logo.size())
            std::cout << logo[i];
        else
            std::cout << std::string(logo[0].size(), ' ');

        std::cout << "   ";

        if (i < (int)info.size())
            std::cout << info[i];

        std::cout << "\n";
    }
}

// ---------------- MAIN ----------------

int main() {
    print();
    return 0;
}
