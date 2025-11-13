#pragma once
#include <string>
#include <vector>
#include "../../../abstractions/serializedToString.h"

struct UnspentTx {
	std::string txid;
	uint32_t vout;
	std::string address;
	std::string scriptPubKey;
	double amount;
	uint64_t confirmations;
	bool spendable;
	bool solvable;
	std::string desc;
	std::vector<std::string> parent_descs;
	bool safe;
};

class ListUnspentResponse : public SerializedToString
{
public:
	std::vector<UnspentTx> unspentTx;
	ListUnspentResponse();
	ListUnspentResponse(const std::string& json);
	~ListUnspentResponse() = default;
	std::string toString() const override;
};