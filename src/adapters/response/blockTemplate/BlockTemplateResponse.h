#pragma once
#include "../../../abstractions/serializedToString.h"
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <optional>
#include <cstdint>
#include "Transaction.h"
#include "CoinBaseTxn.h"

class BlockTemplateResponse : SerializedToString
{
public:
	std::vector<std::string> capabilities;
	std::vector<std::string> rules;
	std::vector<std::string> mutableFields;
	std::vector<Transaction> transactions;
	std::map<std::string, uint8_t> vbavailable;
	std::map<std::string, std::string> coinbaseaux;
	std::string previousblockhash;
	std::optional<uint64_t> coinbasevalue;
	std::string longpollid;
	std::string target;
	int64_t mintime;
	std::string noncerange;
	uint64_t sigoplimit;
	uint64_t sizelimit;
	uint64_t weightlimit;
	int64_t curtime;
	std::string bits;
	uint32_t height;
	std::optional <std::string> defaultWitnessCommitment;
	int32_t version;
	uint32_t vbrequired;
	std::optional<CoinBaseTxn> coinbasetxn;
	
	BlockTemplateResponse(const std::string& json);
	~BlockTemplateResponse();
	std::string toString() const override;
};