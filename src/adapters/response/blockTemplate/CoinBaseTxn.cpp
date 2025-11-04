#include "CoinBaseTxn.h"
#include <json/json.h>

CoinBaseTxn::CoinBaseTxn(std::string data, std::optional<std::string> txid, std::optional<std::string> hash, std::vector<uint32_t> depends, std::optional<int64_t> fee, std::optional<uint64_t> sigops, std::optional<uint64_t> weight, std::optional<bool> required)
{
	this->data = data;
	this->txid = txid;
	this->hash = hash;
	this->depends = depends;
	this->fee = fee;
	this->sigops = sigops;
	this->weight = weight;
	this->required = required;
}

std::string CoinBaseTxn::toString() const
{
	Json::Value json;
	json["data"] = data;
	if (txid.has_value()) {
		json["txid"] = txid.value();
	}
	if (hash.has_value()) {
		json["hash"] = hash.value();
	}
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
	if (required.has_value()) {
		json["required"] = required.value();
	}
	Json::StreamWriterBuilder writer;
	return Json::writeString(writer, json);
}
