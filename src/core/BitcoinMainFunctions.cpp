#include "BitcoinMainFunctions.h"
#include <vector>
#include <cstddef>
#include <string>
#include "../../src/adapters/response/blockTemplate/Transaction.h"
#include "../../src/adapters/response/blockTemplate/BlockTemplateResponse.h"
#include "../../src/adapters/response/listunspent/ListUnspentResponse.h"
#include "../utils/VarInt.h"
#include <span>
#include <boost/algorithm/hex.hpp>
#include <boost/endian/conversion.hpp>
#include <iostream>

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

std::vector<std::byte> BitcoinMainFunctions::calcUnsignedVin(UnspentTx unspentTx) {
	std::vector<std::byte> vin;
	std::vector<std::byte> previousOutput = BitcoinMainFunctions::previousOutput(unspentTx);
	unsigned char scriptLength = 0x00;
	uint32_t sequence = 0xffffffff;
	vin.insert(vin.end(), previousOutput.begin(), previousOutput.end());
	vin.push_back(static_cast<std::byte>(scriptLength));
	std::vector<std::byte> sequenceBytes = BitcoinMainFunctions::uint32ToBytes(sequence);
	vin.insert(vin.end(), sequenceBytes.begin(), sequenceBytes.end());
	return vin;
}

std::vector<std::byte> BitcoinMainFunctions::endian_reverse_vector(std::vector<std::byte> bytes)
{
	std::vector<std::byte> reversedBytes(bytes.size());
	for (size_t i = 0; i < bytes.size(); ++i) {
		reversedBytes[i] = bytes[bytes.size() - 1 - i];
	}
	return reversedBytes;
}

std::vector<std::byte> BitcoinMainFunctions::uint32ToBytes(uint32_t value)
{
	std::vector<std::byte> bytes(4);

	bytes[0] = static_cast<std::byte>(value >> 8 * 3 & 0xFF);
	bytes[1] = static_cast<std::byte>(value >> 8 * 2 & 0xFF);
	bytes[2] = static_cast<std::byte>(value >> 8 * 1 & 0xFF);
	bytes[3] = static_cast<std::byte>(value >> 8 * 0 & 0xFF);

	return bytes;
}

std::vector<std::byte> BitcoinMainFunctions::previousOutput(UnspentTx unspentTx)
{
	std::vector<std::byte> txid = BitcoinMainFunctions::endian_reverse_vector(BitcoinMainFunctions::hexStringToBytes(unspentTx.txid));
	std::vector<std::byte> vout = BitcoinMainFunctions::uint32ToBytes(boost::endian::endian_reverse(unspentTx.vout));
	std::vector<std::byte> previousOutput;
	previousOutput.insert(previousOutput.end(), txid.begin(), txid.end());
	previousOutput.insert(previousOutput.end(), vout.begin(), vout.end());
	return previousOutput;
}

std::vector<std::byte> BitcoinMainFunctions::scriptSig()
{
	return std::vector<std::byte>();
}

std::string BitcoinMainFunctions::determineTypeOutput(UnspentTx unspentTx)
{
	if (unspentTx.desc.empty()) {
		throw std::logic_error("determineTypeOutput: desc is empty");
	}

	if (unspentTx.desc.find("wpkh")) {
		return "P2WPKH";
	}
	else if (unspentTx.desc.find("wsh")) {
		return "P2WSH";
	}
	else if (unspentTx.desc.find("sh(wpkh")) {
		return "P2SH-P2WPKH";
	}
	else if (unspentTx.desc.find("sh(wsh")) {
		return "P2SH-P2WSH";
	}
	else if(unspentTx.desc.find("pkh")){
		return "P2PKH";
	}
	else if (unspentTx.desc.find("tr")) {
		return "P2TR";
	}
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

	bytes[0] = static_cast<std::byte>(value >> 8*7 & 0xFF);
	bytes[1] = static_cast<std::byte>(value >> 8*6 & 0xFF);
	bytes[2] = static_cast<std::byte>(value >> 8*5 & 0xFF);
	bytes[3] = static_cast<std::byte>(value >> 8*4 & 0xFF);
	bytes[4] = static_cast<std::byte>(value >> 8*3 & 0xFF);
	bytes[5] = static_cast<std::byte>(value >> 8*2 & 0xFF);
	bytes[6] = static_cast<std::byte>(value >> 8*1 & 0xFF);
	bytes[7] = static_cast<std::byte>(value >> 8*0 & 0xFF);
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

template <class ByteContainer>
static std::string BitcoinMainFunctions::hex_lower(const ByteContainer& v) {
	std::string s;
	s.reserve(v.size() * 2);
	const unsigned char* first = reinterpret_cast<const unsigned char*>(v.data());
	const unsigned char* last = first + v.size();
	boost::algorithm::hex(first, last, std::back_inserter(s)); // UPPER by default
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return s;
}