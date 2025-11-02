//
// Created by Boris on 02.11.2025.
//
#include <string>

class bitcoinAdapter {
private:
    std::string host_;
    std::string port_;
    std::string user_;
    std::string password_;
public:
    bitcoinAdapter();
    bitcoinAdapter(std::string host,std::string port,std::string user,std::string password);
    ~bitcoinAdapter() = default;
    std::string send(std::string contentType,std::string body);
};