#pragma once
#include <cstddef>   // std::byte
#include <cstdint>
#include <vector>

namespace VarInt {
    std::vector<std::byte> encode_compact_size(std::uint64_t n);

    inline constexpr std::size_t compact_size_len(std::uint64_t n) {
        return (n < 0xFDull) ? 1 :
            (n <= 0xFFFFull) ? 3 :
            (n <= 0xFFFFFFFFull) ? 5 : 9;
    }
}