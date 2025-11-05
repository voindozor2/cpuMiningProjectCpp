#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <cctype>
#include <boost/algorithm/hex.hpp>
#include "../src/utils/VarInt.h"

// bytes -> hex (lowercase), совместимо и с std::byte, и с uint8_t
template <class ByteContainer>
static std::string hex_lower(const ByteContainer& v) {
    std::string s;
    s.reserve(v.size() * 2);
    const unsigned char* first = reinterpret_cast<const unsigned char*>(v.data());
    const unsigned char* last = first + v.size();
    boost::algorithm::hex(first, last, std::back_inserter(s)); // UPPER by default
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

// sanity
int add(int a, int b) { return a + b; }
TEST(AddTest, Works_Positive) { EXPECT_EQ(add(2, 3), 5); }
TEST(AddTest, Works_Negative) { EXPECT_EQ(add(-1, 1), 0); }

// ---- Small (1-byte) ----
TEST(CompactSize_Encode_Small, N_0) {
    auto b = VarInt::encode_compact_size(0);
    EXPECT_EQ(hex_lower(b), "00");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(0));
}
TEST(CompactSize_Encode_Small, N_1) {
    auto b = VarInt::encode_compact_size(1);
    EXPECT_EQ(hex_lower(b), "01");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(1));
}
TEST(CompactSize_Encode_Small, N_252) {
    auto b = VarInt::encode_compact_size(252);
    EXPECT_EQ(hex_lower(b), "fc");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(252));
}

// ---- U16 range (FD + LE16) ----
TEST(CompactSize_Encode_U16, N_253) {
    auto b = VarInt::encode_compact_size(253);
    EXPECT_EQ(hex_lower(b), "fdfd00");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(253));
}
TEST(CompactSize_Encode_U16, N_254) {
    auto b = VarInt::encode_compact_size(254);
    EXPECT_EQ(hex_lower(b), "fdfe00");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(254));
}
TEST(CompactSize_Encode_U16, N_255) {
    auto b = VarInt::encode_compact_size(255);
    EXPECT_EQ(hex_lower(b), "fdff00");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(255));
}
TEST(CompactSize_Encode_U16, N_256) {
    auto b = VarInt::encode_compact_size(256);
    EXPECT_EQ(hex_lower(b), "fd0001");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(256));
}
TEST(CompactSize_Encode_U16, N_515) {
    auto b = VarInt::encode_compact_size(515);
    EXPECT_EQ(hex_lower(b), "fd0302");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(515));
}
TEST(CompactSize_Encode_U16, N_65535) {
    auto b = VarInt::encode_compact_size(65535);
    EXPECT_EQ(hex_lower(b), "fdffff");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(65535));
}

// ---- U32 range (FE + LE32) ----
TEST(CompactSize_Encode_U32, N_65536) {
    auto b = VarInt::encode_compact_size(65536);
    EXPECT_EQ(hex_lower(b), "fe00000100");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(65536));
}
TEST(CompactSize_Encode_U32, N_70000) {
    auto b = VarInt::encode_compact_size(70000);
    EXPECT_EQ(hex_lower(b), "fe70110100");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(70000));
}
TEST(CompactSize_Encode_U32, N_4294967295) {
    auto b = VarInt::encode_compact_size(4294967295ULL);
    EXPECT_EQ(hex_lower(b), "feffffffff");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(4294967295ULL));
}

// ---- U64 range (FF + LE64) ----
TEST(CompactSize_Encode_U64, N_4294967296) {
    auto b = VarInt::encode_compact_size(4294967296ULL);
    EXPECT_EQ(hex_lower(b), "ff0000000001000000");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(4294967296ULL));
}
TEST(CompactSize_Encode_U64, N_Max) {
    auto b = VarInt::encode_compact_size(0xFFFFFFFFFFFFFFFFULL);
    EXPECT_EQ(hex_lower(b), "ffffffffffffffffff");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(0xFFFFFFFFFFFFFFFFULL));
}

// ---- Boundary transitions (дублируют некоторые выше, но явно помечены границы)
TEST(CompactSize_Encode_Boundaries, At_0xFC) {
    auto b = VarInt::encode_compact_size(0xFCu);
    EXPECT_EQ(hex_lower(b), "fc");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(0xFCu));
}
TEST(CompactSize_Encode_Boundaries, At_0xFD) {
    auto b = VarInt::encode_compact_size(0xFDU);
    EXPECT_EQ(hex_lower(b), "fdfd00");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(0xFDU));
}
TEST(CompactSize_Encode_Boundaries, At_0xFFFF) {
    auto b = VarInt::encode_compact_size(0xFFFFU);
    EXPECT_EQ(hex_lower(b), "fdffff");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(0xFFFFU));
}
TEST(CompactSize_Encode_Boundaries, At_0x1_0000) {
    auto b = VarInt::encode_compact_size(0x1'0000U);
    EXPECT_EQ(hex_lower(b), "fe00000100");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(0x1'0000U));
}
TEST(CompactSize_Encode_Boundaries, At_0xFFFF_FFFF) {
    auto b = VarInt::encode_compact_size(0xFFFF'FFFFULL);
    EXPECT_EQ(hex_lower(b), "feffffffff");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(0xFFFF'FFFFULL));
}
TEST(CompactSize_Encode_Boundaries, At_0x1_0000_0000) {
    auto b = VarInt::encode_compact_size(0x1'0000'0000ULL);
    EXPECT_EQ(hex_lower(b), "ff0000000001000000");
    EXPECT_EQ(b.size(), VarInt::compact_size_len(0x1'0000'0000ULL));
}
