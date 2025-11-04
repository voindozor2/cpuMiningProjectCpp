#pragma once
#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include "../../../abstractions/serializedToString.h"

class Transaction : SerializedToString
{
public:
	std::string data;
	std::string txid;
	std::string	hash;
	std::vector<uint32_t> depends;
	std::optional<uint64_t> fee;
	std::optional<uint32_t> sigops;
	std::optional<uint64_t> weight;

	Transaction(std::string data,std::string txid,
		std::string hash, std::vector<uint32_t> depends,
		std::optional<uint64_t> fee, std::optional<uint32_t> sigops,
		std::optional<uint64_t> weight);
	~Transaction() = default;
	std::string toString() const override;
};