//
// Created by Boris on 02.11.2025.
//

#include "bitcoinAdapter.h"
#include <string>
#include <cpr/cpr.h>
#include "../abstractions/serializedToString.h"
#include <iostream>

bitcoinAdapter::bitcoinAdapter()
{
    this->host_ = "";
    this->port_ = "";
    this->user_ =  "";
    this->password_ = "";
}

bitcoinAdapter::bitcoinAdapter(std::string host,std::string port,std::string user,std::string password)
{
    this->host_ = host;
    this->port_ = port;
    this->user_ =  user;
    this->password_ = password;
}

std::string bitcoinAdapter::send(SerializedToString* request)
{
	std::cout << request->toString() << std::endl;
    auto response = cpr::Post(
        cpr::Url{"http://"+host_+ ":" + port_},
        cpr::Authentication{user_, password_,cpr::AuthMode::BASIC},
        cpr::Body{request->toString()},
        cpr::Header{{"content-type", "text/plain"}});
    return response.text;
}
