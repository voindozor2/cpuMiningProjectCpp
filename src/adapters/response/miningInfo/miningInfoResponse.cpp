#include "miningInfoResponse.h"
#include <json/json.h>

MiningInfoResponse::MiningInfoResponse()
{
	this->blocks = 0;
	this-> currentBlockWeigth = 0;
	this-> currentBlockTx = 0;
	this-> difficulty = 0.0;
	this-> networkHashps = 0.0;
	this-> pooledTx = 0;
}

MiningInfoResponse::MiningInfoResponse(const std::string& json)
{
	Json::Value root;
	Json::CharReaderBuilder builder;
	std::string errs;
	std::istringstream iss(json);
	if (!Json::parseFromStream(builder, iss, &root, &errs)) {
		throw std::runtime_error("Failed to parse JSON: " + errs);
	}
	const auto result = root["result"];

	this->blocks = result["blocks"].asInt();
	this->currentBlockWeigth = result["currentblockweight"].asInt();
	this->currentBlockTx = result["currentblocktx"].asInt();
	this->difficulty = result["difficulty"].asDouble();
	this->networkHashps = result["networkhashps"].asDouble();
	this->pooledTx = result["pooledtx"].asInt();
}

std::string MiningInfoResponse::toString() const
{
	Json::Value json;
	json["blocks"] = blocks;
	json["currentblockweight"] = currentBlockWeigth;
	json["currentblocktx"] = currentBlockTx;
	json["difficulty"] = difficulty;
	json["networkhashps"] = networkHashps;
	json["pooledtx"] = pooledTx;

	Json::StreamWriterBuilder writer;
	return Json::writeString(writer, json);
}