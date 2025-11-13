#pragma once
#include <vector>
#include <cstddef>
#include <string>
#include "../../src/adapters/response/blockTemplate/Transaction.h"
#include "../../src/adapters/response/blockTemplate/BlockTemplateResponse.h"
#include "../adapters/response/listunspent/ListUnspentResponse.h"

namespace BitcoinMainFunctions {
	std::vector<std::byte> calcVout(std::string scriptPubKey, uint32_t height, std::vector<Transaction> transactions);
	uint64_t calculateValueSatoshi(uint32_t height, std::vector<Transaction> transactions);
	std::vector<std::byte> stringToBytes(const std::string& str);
	bool isMaxBlockWeight(unsigned int actualWeight);
	uint64_t calcSubsidy(uint32_t height);
	std::vector<std::byte> uint64ToBytes(uint64_t value);
	std::vector<std::byte> hexStringToBytes(const std::string& hex);
	std::vector<std::byte> calcUnsignedVin(UnspentTx unspentTx);
	std::vector<std::byte> endian_reverse_vector(std::vector<std::byte> bytes);
	std::vector<std::byte> uint32ToBytes(uint32_t value);
	std::vector<std::byte> previousOutput(UnspentTx unspentTx);
	std::vector<std::byte> scriptSig();
	std::string determineTypeOutput(UnspentTx unspentTx);
	template <class ByteContainer>
	static std::string hex_lower(const ByteContainer& v);
}

