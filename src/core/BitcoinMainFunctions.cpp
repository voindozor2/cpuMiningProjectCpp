#include "BitcoinMainFunctions.h"
#include <vector>
#include <cstddef>
#include <string>
#include "../../src/adapters/response/blockTemplate/Transaction.h"
#include "../../src/adapters/response/blockTemplate/BlockTemplateResponse.h"
#include "../utils/VarInt.h"
#include <span>
#include <boost/algorithm/hex.hpp>
#include <boost/endian/conversion.hpp>

std::vector<std::byte> BitcoinMainFunctions::calcVout(std::string scriptPubKey, uint32_t height, std::vector<Transaction> transactions)
{
	std::vector<std::byte> vout;
	uint64_t value = boost::endian::endian_reverse(calculateValueSatoshi(height, transactions));
	std::vector<std::byte> valueBytes = uint64ToBytes(value);
	vout.insert(vout.end(), valueBytes.begin(), valueBytes.end());
	std::vector<std::byte> scriptPubKeyBytes = hexStringToBytes(scriptPubKey);
	std::vector<std::byte> varInt = VarInt::encode_compact_size(scriptPubKeyBytes.size());
	vout.insert(vout.end(), varInt.begin(), varInt.end());
	vout.insert(vout.end(), scriptPubKeyBytes.begin(), scriptPubKeyBytes.end());
	return vout;
}

uint64_t BitcoinMainFunctions::calculateValueSatoshi(uint32_t height, std::vector<Transaction> transactions) { //Тут сделано допущение, нету отбора транзакций по эффективной комиссии от их веса
	uint64_t totalFees = 0;
	uint64_t totalWeight = 0;
	for(int i = 0; i < transactions.size(); i++) {
		if(transactions[i].fee.has_value() && transactions[i].weight.has_value()) { 
			if (!isMaxBlockWeight(totalWeight + transactions[i].weight.value())) {
				totalWeight += transactions[i].weight.value();
				totalFees += transactions[i].fee.value();
			}
		}
	}
	return totalFees + calcSubsidy(height);
}

std::vector<std::byte> BitcoinMainFunctions::stringToBytes(const std::string& str) {
	std::span<const char> chars{ str.data(), str.size() };
	auto bytes_view = std::as_bytes(chars);              // span<const std::byte>

	// 2) Копируем view в собственный буфер-веткор
	std::vector<std::byte> bytes(bytes_view.begin(), bytes_view.end());
	return bytes;
}

bool BitcoinMainFunctions::isMaxBlockWeight(unsigned int actualWeight) {
	if(actualWeight >= 4000000) {
		return true;
	}
	return false;
}

uint64_t BitcoinMainFunctions::calcSubsidy(uint32_t height) {
	uint64_t coin = 100000000; //satoshi in one coin	
	uint64_t halvingInterval = 210000u; //satoshi in one coin	
	uint64_t halvings = height / halvingInterval;
	if (halvings < 64) {
		return (50 * coin) >> halvings;
	}
	else {
		return 0;
	}
}

std::vector<std::byte> BitcoinMainFunctions::uint64ToBytes(uint64_t value) {
	std::vector<std::byte> bytes(8);
	for (size_t i = 0; i < 8; ++i) {
		int shift = (7 - i) * 8;
		bytes[i] = static_cast<std::byte>((value >> shift) & 0xFF);
	}
	return bytes;
}

std::vector<std::byte> BitcoinMainFunctions::hexStringToBytes(const std::string& hex) {
	if ((hex.size() & 1u) != 0) {
		throw std::invalid_argument("hexStringToBytes: hex length must be even");
	}
	for (unsigned char c : hex) {
		if (!std::isxdigit(c)) {
			throw std::invalid_argument("hexStringToBytes: non-hex character");
		}
	}

	std::vector<unsigned char> tmp;
	tmp.reserve(hex.size() / 2);
	boost::algorithm::unhex(hex.begin(), hex.end(), std::back_inserter(tmp));

	std::vector<std::byte> out(tmp.size());
	std::transform(tmp.begin(), tmp.end(), out.begin(),
		[](unsigned char b) { return static_cast<std::byte>(b); });
	return out;
}