#include "BlockTemplateResponse.h"
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <optional>
#include <cstdint>
#include "Transaction.h"
#include "CoinBaseTxn.h"
#include <json/json.h>

BlockTemplateResponse::BlockTemplateResponse(const std::string& json)
{
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream iss(json);
    if (!Json::parseFromStream(builder, iss, &root, &errs)) {
        throw std::runtime_error("Failed to parse JSON: " + errs);
    }

    const auto& result = root["result"];

	Json::Value capabilitiesJson = result["capabilities"];
    for (const auto& item : capabilitiesJson) {
        capabilities.push_back(item.asString());
	}

    Json::Value rulesJson = result["rules"];
    for (const auto& item : rulesJson) {
        rules.push_back(item.asString());
    }

    Json::Value mutableJson = result["mutable"];
    for (const auto& item : mutableJson) {
        mutableFields.push_back(item.asString());
    }

    Json::Value transactionsJson = result["transactions"];
    for (const Json::Value item : transactionsJson) {
		std::string data = item["data"].asString();
		std::string txid = item["txid"].asString();
		std::string hash = item["hash"].asString();
		std::vector<uint32_t> depends;
		Json::Value dependsJson = item["depends"];
        for (const auto& dep : dependsJson) {
            depends.push_back(dep.asUInt());
        }
		std::optional<uint64_t> fee;
        if (item.isMember("fee")) {
            fee = item["fee"].asUInt64();
        }
		std::optional<uint32_t> sigops;
        if (item.isMember("sigops")) {
            sigops = item["sigops"].asUInt();
		}
        std::optional<uint64_t> weight;
        if (item.isMember("weight")) {
            weight = item["weight"].asUInt64();
		}
		transactions.push_back(Transaction(data, txid, hash, depends, fee, sigops, weight));
    }

	Json::Value vbavailableJson = result["vbavailable"];
    for (const auto& key : vbavailableJson.getMemberNames()) {
        vbavailable[key] = static_cast<uint8_t>(vbavailableJson[key].asUInt());
    }
	Json::Value coinbaseauxJson = result["coinbaseaux"];
    for (const auto& key : coinbaseauxJson.getMemberNames()) {
        coinbaseaux[key] = coinbaseauxJson[key].asString();
	}
    previousblockhash = result["previousblockhash"].asString();
    if (result.isMember("coinbasevalue")) {
        coinbasevalue = result["coinbasevalue"].asUInt64();
	}
    longpollid = result["longpollid"].asString();
    target = result["target"].asString();
    mintime = result["mintime"].asUInt64();
    noncerange = result["noncerange"].asString();
    sigoplimit = result["sigoplimit"].asUInt64();
    sizelimit = result["sizelimit"].asUInt64();
    weightlimit = result["weightlimit"].asUInt64();
    curtime = result["curtime"].asUInt64();
    bits = result["bits"].asString();
    height = result["height"].asUInt64();
    version = result["version"].asUInt64();
    vbrequired = result["version"].asUInt64();
    if (result.isMember("defaultWitnessCommitment")) {
        defaultWitnessCommitment = result["defaultWitnessCommitment"].asString();
    }
    if (result.isMember("coinbasetxn")) {
        Json::Value coinbasetxnJson = result["coinbasetxn"];
        std::string data = coinbasetxnJson["data"].asString();
        std::string txid = coinbasetxnJson["txid"].asString();
        std::string hash = coinbasetxnJson["hash"].asString();
        std::vector<uint32_t> depends;
        Json::Value dependsJson = coinbasetxnJson["depends"];
        for (const auto& dep : dependsJson) {
            depends.push_back(dep.asUInt());
        }
        std::optional<uint64_t> fee;
        if (coinbasetxnJson.isMember("fee")) {
            fee = coinbasetxnJson["fee"].asUInt64();
        }
        std::optional<uint32_t> sigops;
        if (coinbasetxnJson.isMember("sigops")) {
            sigops = coinbasetxnJson["sigops"].asUInt();
        }
        std::optional<uint64_t> weight;
        if (coinbasetxnJson.isMember("weight")) {
            weight = coinbasetxnJson["weight"].asUInt64();
        }

		std::optional<bool> required;
        if (coinbasetxnJson.isMember("required")) {
            required = coinbasetxnJson["required"].asBool();
		}
		coinbasetxn = CoinBaseTxn(data,txid,hash,depends,fee,sigops,weight,required);
    }
}

BlockTemplateResponse::~BlockTemplateResponse() = default;

std::string BlockTemplateResponse::toString() const
{
    Json::Value json;
	json["capabilities"] = Json::Value(Json::arrayValue);
    for (const auto& item : capabilities) {
        json["capabilities"].append(item);
	}
    json["rules"] = Json::Value(Json::arrayValue);
    for (const auto& item : rules) {
        json["rules"].append(item);
    }
    json["mutable"] = Json::Value(Json::arrayValue);
    for (const auto& item : mutableFields) {
        json["mutable"].append(item);
    }
    json["transactions"] = Json::Value(Json::arrayValue);
    for (const auto& txn : transactions) {
        Json::Value txnJson;
        std::istringstream iss(txn.toString());
        iss >> txnJson;
        json["transactions"].append(txnJson);
    }
    Json::Value vbavailableJson;
    for (const auto& [key, value] : vbavailable) {
        vbavailableJson[key] = value;
    }
    json["vbavailable"] = vbavailableJson;
    Json::Value coinbaseauxJson;
    for (const auto& [key, value] : coinbaseaux) {
        coinbaseauxJson[key] = value;
    }
    json["coinbaseaux"] = coinbaseauxJson;
    json["previousblockhash"] = previousblockhash;
    if (coinbasevalue.has_value()) {
        json["coinbasevalue"] = coinbasevalue.value();
    }
    json["longpollid"] = longpollid;
    json["target"] = target;
    json["mintime"] = static_cast<Json::UInt64>(mintime);
    json["noncerange"] = noncerange;
    json["sigoplimit"] = static_cast<Json::UInt64>(sigoplimit);
    json["sizelimit"] = static_cast<Json::UInt64>(sizelimit);
    json["weightlimit"] = static_cast<Json::UInt64>(weightlimit);
    json["curtime"] = static_cast<Json::UInt64>(curtime);
    json["bits"] = bits;
    json["height"] = static_cast<Json::UInt64>(height);
    if (defaultWitnessCommitment.has_value()) {
        json["defaultWitnessCommitment"] = defaultWitnessCommitment.value();
    }
    json["version"] = static_cast<Json::UInt64>(version);
    json["vbrequired"] = static_cast<Json::UInt64>(vbrequired);
    if (coinbasetxn.has_value()) {
        Json::Value coinbasetxnJson;
		std::istringstream
			iss(coinbasetxn.value().toString());
        iss >> coinbasetxnJson;
        json["coinbasetxn"] = coinbasetxnJson;
    }
    Json::StreamWriterBuilder writer;
	return Json::writeString(writer, json);
}

BlockTemplateResponse::BlockTemplateResponse(
    std::vector<std::string> capabilities,
    std::vector<std::string> rules,
    std::vector<std::string> mutableFields,
    std::vector<Transaction> transactions,
    std::map<std::string, uint8_t> vbavailable,
    std::map<std::string, std::string> coinbaseaux,
    std::string previousblockhash,
    std::optional<uint64_t> coinbasevalue,
    std::string longpollid,
    std::string target,
    int64_t mintime,
    std::string noncerange,
    uint64_t sigoplimit,
    uint64_t sizelimit,
    uint64_t weightlimit,
    int64_t curtime,
    std::string bits,
    uint32_t height,
    std::optional<std::string> defaultWitnessCommitment,
    int32_t version,
    uint32_t vbrequired,
    std::optional<CoinBaseTxn> coinbasetxn
) 
{
    this->capabilities = capabilities;
    this->rules = rules;
    this->mutableFields = mutableFields;
    this->transactions = transactions;
    this->vbavailable = vbavailable;
    this->coinbaseaux = coinbaseaux;
    this->previousblockhash = previousblockhash;
    this->coinbasevalue = coinbasevalue;
    this->longpollid = longpollid;
    this->target = target;
    this->mintime = mintime;
    this->noncerange = noncerange;
    this->sigoplimit = sigoplimit;
    this->sizelimit = sizelimit;
    this->weightlimit = weightlimit;
    this->curtime = curtime;
    this->bits = bits;
    this->height = height;
    this->defaultWitnessCommitment = defaultWitnessCommitment;
    this->version = version;
    this->vbrequired = vbrequired;
	this->coinbasetxn = coinbasetxn;
}