#pragma once
#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include "../../../abstractions/serializedToString.h"

class CoinBaseTxn : SerializedToString
{
public:
	std::string data;
	std::optional <std::string>	txid;
	std::optional <std::string> hash;
	std::vector<uint32_t> depends;
	std::optional <int64_t> fee;
	std::optional <uint64_t> sigops;
	std::optional <uint64_t> weight;
	std::optional<bool> required;

	CoinBaseTxn(std::string data,std::optional <std::string> txid,
		std::optional <std::string> hash,std::vector<uint32_t> depends,
		std::optional <int64_t> fee,std::optional <uint64_t> sigops,
		std::optional <uint64_t> weight,std::optional<bool> required);
	~CoinBaseTxn() = default;
	std::string toString() const override;
};