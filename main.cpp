#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <openssl/sha.h>

using namespace std;

struct Params {
    string host = "127.0.0.1";
    int port = 33333;
    string inFile;
    string outFile;
    string login = "user";
    string password = "pass";
};

string sha1(const string& data) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);
    char buf[41];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(buf + (i * 2), "%02x", hash[i]);
    }
    return string(buf);
}

vector<vector<float>> readData(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) throw runtime_error("File error");
    int count;
    if (!(file >> count)) throw runtime_error("Read error");
    vector<vector<float>> data(count);
    for (int i = 0; i < count; ++i) {
        int size;
        file >> size;
        data[i].resize(size);
        for (int j = 0; j < size; ++j) {
            file >> data[i][j];
        }
    }
    return data;
}

void writeResult(const string& filename, const vector<float>& results) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) throw runtime_error("Write error");
    uint32_t count = results.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (float val : results) {
        file.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
}

void sendAll(int sock, const void* data, size_t len) {
    const char* ptr = static_cast<const char*>(data);
    while (len > 0) {
        ssize_t sent = send(sock, ptr, len, 0);
        if (sent <= 0) throw runtime_error("Send error");
        ptr += sent;
        len -= sent;
    }
}

void recvAll(int sock, void* data, size_t len) {
    char* ptr = static_cast<char*>(data);
    while (len > 0) {
        ssize_t recvd = recv(sock, ptr, len, 0);
        if (recvd <= 0) throw runtime_error("Recv error");
        ptr += recvd;
        len -= recvd;
    }
}

int main(int argc, char* argv[]) {
    try {
        Params p;
        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];
            if (arg == "-a" && i + 1 < argc) p.host = argv[++i];
            else if (arg == "-p" && i + 1 < argc) p.port = stoi(argv[++i]);
            else if (arg == "-i" && i + 1 < argc) p.inFile = argv[++i];
            else if (arg == "-o" && i + 1 < argc) p.outFile = argv[++i];
            else if (arg == "-u" && i + 1 < argc) p.login = argv[++i];
            else if (arg == "-w" && i + 1 < argc) p.password = argv[++i];
        }

        if (p.inFile.empty() || p.outFile.empty()) {
            cerr << "Usage: ./client -i input.txt -o output.bin [-a host] [-p port] [-u login] [-w pass]" << endl;
            return 1;
        }

        auto data = readData(p.inFile);

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) throw runtime_error("Socket error");

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(p.port);
        inet_pton(AF_INET, p.host.c_str(), &serverAddr.sin_addr);

        if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            throw runtime_error("Connect error");
        }

        uint32_t loginLen = p.login.size();
        sendAll(sock, &loginLen, sizeof(loginLen));
        sendAll(sock, p.login.c_str(), loginLen);

        uint32_t saltLen;
        recvAll(sock, &saltLen, sizeof(saltLen));
        string salt(saltLen, '\0');
        recvAll(sock, &salt[0], saltLen);

        string hash = sha1(salt + p.password);
        uint32_t hashLen = hash.size();
        sendAll(sock, &hashLen, sizeof(hashLen));
        sendAll(sock, hash.c_str(), hashLen);

        char status[4];
        recvAll(sock, status, 3);
        if (string(status, 3) != "OK") {
            throw runtime_error("Auth error");
        }

        vector<float> results;
        results.reserve(data.size());

        for (const auto& vec : data) {
            uint32_t vecSize = vec.size();
            sendAll(sock, &vecSize, sizeof(vecSize));
            sendAll(sock, vec.data(), vecSize * sizeof(float));
            float res;
            recvAll(sock, &res, sizeof(float));
            results.push_back(res);
        }

        close(sock);
        writeResult(p.outFile, results);

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
