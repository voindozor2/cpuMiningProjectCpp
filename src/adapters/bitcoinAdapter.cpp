//
// Created by Boris on 02.11.2025.
//

#include "bitcoinAdapter.h"
#include <string>
#include <cpr/cpr.h>
#include "../abstractions/serializedToString.h"
#include <iostream>
#include "response/blockCount/blockCountResponse.h"
#include "response/miningInfo/miningInfoResponse.h"
#include "response/submitBlock/submitBlockResponse.h"
#include "requests/blockCount/blockCountRequest.h"
#include "requests/miningInfo/miningInfoRequest.h"
#include "requests/submitBlock/submitBlockRequest.h"

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
    auto response = cpr::Post(
        cpr::Url{"http://"+host_+ ":" + port_},
        cpr::Authentication{user_, password_,cpr::AuthMode::BASIC},
        cpr::Body{request->toString()},
        cpr::Header{{"content-type", "text/plain"}});
    return response.text;
}

BlockCountResponse bitcoinAdapter::getBlockCount()
{
    BlockCountRequest blockCountRequest;
	return BlockCountResponse(send(&blockCountRequest));
}

MiningInfoResponse bitcoinAdapter::getMiningInfo()
{
	MiningInfoRequest miningInfoRequest;
    return MiningInfoResponse(send(&miningInfoRequest));
}

SubmitBlockResponse bitcoinAdapter::submitBlock(const std::string blockData)
{
	SubmitBlockRequest submitBlockRequest(blockData);
    return SubmitBlockResponse(send(&submitBlockRequest));
}
