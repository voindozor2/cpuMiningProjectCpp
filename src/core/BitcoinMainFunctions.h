#pragma once
#include <vector>
#include <cstddef>
#include <string>
#include "../../src/adapters/response/blockTemplate/Transaction.h"
#include "../../src/adapters/response/blockTemplate/BlockTemplateResponse.h"

namespace BitcoinMainFunctions {
	std::vector<std::byte> calcVout(std::string scriptPubKey, uint32_t height, std::vector<Transaction> transactions);
	uint64_t calculateValueSatoshi(uint32_t height, std::vector<Transaction> transactions);
	std::vector<std::byte> stringToBytes(const std::string& str);
	bool isMaxBlockWeight(unsigned int actualWeight);
	uint64_t calcSubsidy(uint32_t height);
	std::vector<std::byte> uint64ToBytes(uint64_t value);
	std::vector<std::byte> hexStringToBytes(const std::string& hex);
}