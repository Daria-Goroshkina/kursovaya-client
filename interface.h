#pragma once
#include <string>

struct Params {
    std::string host = "127.0.0.1";
    int port = 33333;
    std::string inFile;
    std::string outFile;
    std::string login = "user";
    std::string password = "pass";
};

class Interface {
public:
    bool parseArgs(int argc, char* argv[], Params& params);
    void printHelp();
};
