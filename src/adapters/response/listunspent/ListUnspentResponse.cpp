#include "ListUnspentResponse.h"
#include <json/json.h>

ListUnspentResponse::ListUnspentResponse()
{
	this->unspentTx = std::vector<UnspentTx>();
}

ListUnspentResponse::ListUnspentResponse(const std::string& json)
{
	this->unspentTx = std::vector<UnspentTx>();
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream iss(json);
    if (!Json::parseFromStream(builder, iss, &root, &errs)) {
        throw std::runtime_error("Failed to parse JSON: " + errs);
    }

	const auto result = root["result"];

    for (const auto& item : result) {
		UnspentTx tx;
		tx.txid = item["txid"].asString();
		tx.vout = item["vout"].asUInt64();
		tx.address = item["address"].asString();
		tx.scriptPubKey = item["scriptPubKey"].asString();
		tx.amount = item["amount"].asDouble();
		tx.confirmations = item["confirmations"].asUInt64();
		tx.spendable = item["spendable"].asBool();
		tx.solvable = item["solvable"].asBool();
		tx.desc = item["desc"].asString();
		tx.parent_descs = std::vector<std::string>();
		const auto& parent_descs = item["parent_descs"];
		if (item["parent_descs"].isArray()) {
			for (const auto& parent_descs : item["parent_descs"]) {
				std::string desc = parent_descs.asString();
				tx.parent_descs.push_back(desc);
			}
		}
		tx.safe = item["safe"].asBool();
		this->unspentTx.push_back(tx);
    }

}

std::string ListUnspentResponse::toString() const
{
	Json::Value json = Json::Value(Json::arrayValue);
	for (const auto& tx : unspentTx) {
		Json::Value txJson;
		txJson["txid"] = tx.txid;
		txJson["vout"] = static_cast<Json::UInt64>(tx.vout);
		txJson["address"] = tx.address;
		txJson["scriptPubKey"] = tx.scriptPubKey;
		txJson["amount"] = tx.amount;
		txJson["confirmations"] = static_cast<Json::UInt64>(tx.confirmations);
		txJson["spendable"] = tx.spendable;
		txJson["solvable"] = tx.solvable;
		txJson["desc"] = tx.desc;
		txJson["parent_descs"] = Json::Value(Json::arrayValue);
		for (const auto& desc : tx.parent_descs) {
			txJson["parent_descs"].append(desc);
		}
		txJson["safe"] = tx.safe;
		json.append(txJson);
	}
	Json::StreamWriterBuilder writer;
	return Json::writeString(writer, json);
}
