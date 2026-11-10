#include "interface.h"
#include <iostream>

bool Interface::parseArgs(int argc, char* argv[], Params& params) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") return false;
        else if (arg == "-a" && i + 1 < argc) params.host = argv[++i];
        else if (arg == "-p" && i + 1 < argc) params.port = std::stoi(argv[++i]);
        else if (arg == "-i" && i + 1 < argc) params.inFile = argv[++i];
        else if (arg == "-o" && i + 1 < argc) params.outFile = argv[++i];
        else if (arg == "-u" && i + 1 < argc) params.login = argv[++i];
        else if (arg == "-w" && i + 1 < argc) params.password = argv[++i];
    }
    if (params.inFile.empty() || params.outFile.empty()) return false;
    return true;
}

void Interface::printHelp() {
    std::cout << "Usage: ./client -i <input.txt> -o <output.bin> [-a host] [-p port] [-u login] [-w pass]" << std::endl;
}
