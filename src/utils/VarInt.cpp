#pragma once
#include <vector>
#include <cstdint>

namespace VarInt {
	std::vector<std::byte> encode_compact_size(uint64_t toVarInt) {
		std::vector<std::byte> result;

		if (toVarInt < 0xFD)
		{
			result.push_back(static_cast<std::byte>(toVarInt));
		}
		else if (toVarInt <= 0xFFFF)
		{
			result.push_back(static_cast<std::byte>(0xFD));
			result.push_back(static_cast<std::byte>(toVarInt & 0xFF));
			result.push_back(static_cast<std::byte>((toVarInt >> 8) & 0xFF));
		}
		else if (toVarInt <= 0xFFFFFFFF)
		{
			result.push_back(static_cast<std::byte>(0xFE));
			result.push_back(static_cast<std::byte>((toVarInt >> 0) & 0xFF));
			result.push_back(static_cast<std::byte>((toVarInt >> 8) & 0xFF));
			result.push_back(static_cast<std::byte>((toVarInt >> 16) & 0xFF));
			result.push_back(static_cast<std::byte>((toVarInt >> 24) & 0xFF));
		}
		else
		{
			result.push_back(static_cast<std::byte>(0xFF));
			result.push_back(static_cast<std::byte>((toVarInt >> 0) & 0xFF));
			result.push_back(static_cast<std::byte>((toVarInt >> 8) & 0xFF));
			result.push_back(static_cast<std::byte>((toVarInt >> 16) & 0xFF));
			result.push_back(static_cast<std::byte>((toVarInt >> 24) & 0xFF));
			result.push_back(static_cast<std::byte>((toVarInt >> 32) & 0xFF));
			result.push_back(static_cast<std::byte>((toVarInt >> 40) & 0xFF));
			result.push_back(static_cast<std::byte>((toVarInt >> 48) & 0xFF));
			result.push_back(static_cast<std::byte>((toVarInt >> 56) & 0xFF));
		}
		return result;
	}
}