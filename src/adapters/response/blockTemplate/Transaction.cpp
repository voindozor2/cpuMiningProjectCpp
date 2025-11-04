#include "Transaction.h"
#include <json/json.h>

Transaction::Transaction(std::string data, std::string txid, std::string hash, std::vector<uint32_t> depends, std::optional<uint64_t> fee, std::optional<uint32_t> sigops, std::optional<uint64_t> weight)
{
	this->data = data;
	this->txid = txid;
	this->hash = hash;
	this->depends = depends;
	this->fee = fee;
	this->sigops = sigops;
	this->weight = weight;
}

std::string Transaction::toString() const
{
	Json::Value json;
	json["data"] = data;
	json["txid"] = txid;
	json["hash"] = hash;
	Json::Value dependsJson(Json::arrayValue);
	for (const auto& dep : depends) {
		dependsJson.append(dep);
	}
	json["depends"] = dependsJson;
	if (fee.has_value()) {
		json["fee"] = fee.value();
	}
	if (sigops.has_value()) {
		json["sigops"] = sigops.value();
	}
	if (weight.has_value()) {
		json["weight"] = weight.value();
	}
	Json::StreamWriterBuilder writer;
	return Json::writeString(writer, json);
}
