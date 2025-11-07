#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <cctype>
#include <boost/algorithm/hex.hpp>
#include "../src/utils/VarInt.h"
#include "../src/core/BitcoinMainFunctions.h"
#include "../src/adapters/response/blockTemplate/BlockTemplateResponse.h"
#include "../src/adapters/response/blockTemplate/transaction.h"

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

TEST(Calculation_vout, K1) {
    std::vector<Transaction> txsK1 = {
        Transaction("0200000000010101",
            "a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1",
            "b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1",
            {}, std::optional<uint64_t>{9000},  std::optional<uint32_t>{0}, std::optional<uint64_t>{1200}),
        Transaction("0200000000010202",
            "c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2",
            "d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2",
            {}, std::optional<uint64_t>{14000}, std::optional<uint32_t>{0}, std::optional<uint64_t>{1600}),
        Transaction("0200000000010303",
            "e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3",
            "f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3",
            {}, std::optional<uint64_t>{3000},  std::optional<uint32_t>{0}, std::optional<uint64_t>{800})
    };

    std::vector<std::byte> vout = BitcoinMainFunctions::calcVout("76a91488ac", 100000, txsK1);
    ASSERT_EQ(vout.size(), size_t(14));

    const std::byte expected_value[8] = {
        std::byte{0x90}, std::byte{0x57}, std::byte{0x06}, std::byte{0x2a},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}
    };
    for (int i = 0; i < 8; ++i) EXPECT_EQ(vout[i], expected_value[i]) << "value byte " << i;

    EXPECT_EQ(vout[8], std::byte{ 0x05 });
    const std::byte expected_script[5] = {
        std::byte{0x76}, std::byte{0xA9}, std::byte{0x14}, std::byte{0x88}, std::byte{0xAC}
    };
    for (int i = 0; i < 5; ++i) EXPECT_EQ(vout[9 + i], expected_script[i]) << "script byte " << i;

    EXPECT_EQ(hex_lower(vout), "9057062a010000000576a91488ac");
}


TEST(Calculation_vout, K2) {
    std::vector<Transaction> txsK2 = {
    Transaction(
        "0200000000010a0a","111122223333444455556666777788889999aaaabbbbccccddddeeeeffff0000",
        "0000fff0eee0ddd0ccc0bbb0aaa0999988887777666655554444333322221111",
        {}, std::optional<uint64_t>{1'000'000}, std::optional<uint32_t>{0}, std::optional<uint64_t>{10'000})
    };
    // scriptPubKey — hex (декодится в байты внутри calcVout)
    std::vector<std::byte> vout = BitcoinMainFunctions::calcVout("76a91488ac", 100000, txsK2);

    // 1) Общая длина: 8 (value) + 1 (varint) + 5 (script) = 14 байт
    ASSERT_EQ(vout.size(), size_t(14));

    // 2) Побайтовая проверка value (LE8) = 5_001_000_000
    const std::byte expected_value[8] = {
        std::byte{0x40}, std::byte{0x34}, std::byte{0x15}, std::byte{0x2a},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}
    };
    for (int i = 0; i < 8; ++i) {
        EXPECT_EQ(vout[i], expected_value[i]) << "mismatch at value byte " << i;
    }

    // 3) VarInt длины скрипта = 0x05
    EXPECT_EQ(vout[8], std::byte{ 0x05 });

    // 4) Побайтовая проверка scriptPubKey = 76 a9 14 88 ac
    const std::byte expected_script[5] = {
        std::byte{0x76}, std::byte{0xA9}, std::byte{0x14}, std::byte{0x88}, std::byte{0xAC}
    };
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(vout[9 + i], expected_script[i]) << "mismatch at script byte " << i;
    }

	std::string hexLow = hex_lower(vout);
    // 5) Дополнительно: сверка целиком через hex
    EXPECT_EQ(hexLow, std::string("4034152a010000000576a91488ac"));
}

TEST(Calculation_vout, K3) {
    std::vector<Transaction> txsK3 = {
        Transaction("0200000000010b0b","aa...01aa","bb...01bb",{}, std::optional<uint64_t>{9000},  std::optional<uint32_t>{0}, std::optional<uint64_t>{1200}),
        Transaction("0200000000010c0c","aa...02aa","bb...02bb",{}, std::optional<uint64_t>{14000}, std::optional<uint32_t>{0}, std::optional<uint64_t>{1600}),
        Transaction("0200000000010d0d","aa...03aa","bb...03bb",{}, std::optional<uint64_t>{3000},  std::optional<uint32_t>{0}, std::optional<uint64_t>{800})
    };

    std::vector<std::byte> vout = BitcoinMainFunctions::calcVout("76a91488ac", 100000, txsK3);
    ASSERT_EQ(vout.size(), size_t(14));

    const std::byte expected_value[8] = {
        std::byte{0x90}, std::byte{0x57}, std::byte{0x06}, std::byte{0x2a},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}
    };
    for (int i = 0; i < 8; ++i) EXPECT_EQ(vout[i], expected_value[i]);

    EXPECT_EQ(vout[8], std::byte{ 0x05 });
    const std::byte expected_script[5] = {
        std::byte{0x76}, std::byte{0xA9}, std::byte{0x14}, std::byte{0x88}, std::byte{0xAC}
    };
    for (int i = 0; i < 5; ++i) EXPECT_EQ(vout[9 + i], expected_script[i]);

    EXPECT_EQ(hex_lower(vout), "9057062a010000000576a91488ac");
}


TEST(Calculation_vout, K4) {
    std::vector<Transaction> txsK4; // пусто

    std::vector<std::byte> vout = BitcoinMainFunctions::calcVout("76a91488ac", 100000, txsK4);
    ASSERT_EQ(vout.size(), size_t(14));

    const std::byte expected_value[8] = {
        std::byte{0x00}, std::byte{0xF2}, std::byte{0x05}, std::byte{0x2A},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}
    };
    for (int i = 0; i < 8; ++i) EXPECT_EQ(vout[i], expected_value[i]);

    EXPECT_EQ(vout[8], std::byte{ 0x05 });
    const std::byte expected_script[5] = {
        std::byte{0x76}, std::byte{0xA9}, std::byte{0x14}, std::byte{0x88}, std::byte{0xAC}
    };
    for (int i = 0; i < 5; ++i) EXPECT_EQ(vout[9 + i], expected_script[i]);

    EXPECT_EQ(hex_lower(vout), "00f2052a010000000576a91488ac");
}


TEST(Calculation_vout, K5) {
    std::vector<Transaction> txsK5 = {
        Transaction("0200000000010e0e",
            "1111aaaa2222bbbb3333cccc4444dddd5555eeee6666ffff7777888899990000",
            "0000999988887777666655554444333322221111ffffeeee0000ddddccccbbbb",
            {}, std::optional<uint64_t>{1000}, std::optional<uint32_t>{0}, std::optional<uint64_t>{500})
    };

    std::vector<std::byte> vout = BitcoinMainFunctions::calcVout("76a91488ac", 100000, txsK5);
    ASSERT_EQ(vout.size(), size_t(14));

    const std::byte expected_value[8] = {
        std::byte{0xE8}, std::byte{0xF5}, std::byte{0x05}, std::byte{0x2A},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}
    };
    for (int i = 0; i < 8; ++i) EXPECT_EQ(vout[i], expected_value[i]);

    EXPECT_EQ(vout[8], std::byte{ 0x05 });
    const std::byte expected_script[5] = {
        std::byte{0x76}, std::byte{0xA9}, std::byte{0x14}, std::byte{0x88}, std::byte{0xAC}
    };
    for (int i = 0; i < 5; ++i) EXPECT_EQ(vout[9 + i], expected_script[i]);

    EXPECT_EQ(hex_lower(vout), "e8f5052a010000000576a91488ac");
}


TEST(Calculation_vout, K6) {
    std::vector<Transaction> txsK6 = {
        Transaction("0200000000010f0f",
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
            {}, std::optional<uint64_t>{12345}, std::optional<uint32_t>{0}, std::optional<uint64_t>{900})
    };

    std::vector<std::byte> vout = BitcoinMainFunctions::calcVout("76a91488ac", 100000, txsK6);
    ASSERT_EQ(vout.size(), size_t(14));

    const std::byte expected_value[8] = {
        std::byte{0x39}, std::byte{0x22}, std::byte{0x06}, std::byte{0x2A},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}
    };
    for (int i = 0; i < 8; ++i) EXPECT_EQ(vout[i], expected_value[i]);

    EXPECT_EQ(vout[8], std::byte{ 0x05 });
    const std::byte expected_script[5] = {
        std::byte{0x76}, std::byte{0xA9}, std::byte{0x14}, std::byte{0x88}, std::byte{0xAC}
    };
    for (int i = 0; i < 5; ++i) EXPECT_EQ(vout[9 + i], expected_script[i]);

    EXPECT_EQ(hex_lower(vout), "3922062a010000000576a91488ac");
}


TEST(Calculation_vout, K7) {
    std::vector<Transaction> txsK7 = {
        Transaction(
            "02000000000101"
            "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff"
            "00000000"
            "00"
            "ffffffff"
            "01"
            "a086010000000000"
            "01"
            "6a"
            "00000000",
            "2f3c0d9a1b4e5f6789abcdef0123456789abcdef0123456789abcdef01234567",
            "7f6e5d4c3b2a1908ffeeddccbbaa99887766554433221100ffeeddccbbaa9988",
            {}, std::optional<uint64_t>{10'000}, std::optional<uint32_t>{0}, std::optional<uint64_t>{2'000})
    };

    std::vector<std::byte> vout = BitcoinMainFunctions::calcVout("76a91488ac", 100000, txsK7);
    ASSERT_EQ(vout.size(), size_t(14));

    const std::byte expected_value[8] = {
        std::byte{0x10}, std::byte{0x19}, std::byte{0x06}, std::byte{0x2A},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}
    };
    for (int i = 0; i < 8; ++i) EXPECT_EQ(vout[i], expected_value[i]);

    EXPECT_EQ(vout[8], std::byte{ 0x05 });
    const std::byte expected_script[5] = {
        std::byte{0x76}, std::byte{0xA9}, std::byte{0x14}, std::byte{0x88}, std::byte{0xAC}
    };
    for (int i = 0; i < 5; ++i) EXPECT_EQ(vout[9 + i], expected_script[i]);

    EXPECT_EQ(hex_lower(vout), "1019062a010000000576a91488ac");
}


TEST(Calculation_vout, K8) {
    std::vector<Transaction> txsK8 = {
        Transaction("0200000000011212",
            "0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab",
            "abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567",
            {}, std::optional<uint64_t>{7000}, std::optional<uint32_t>{0}, std::optional<uint64_t>{1000})
    };

    std::vector<std::byte> vout = BitcoinMainFunctions::calcVout("76a91488ac", 100000, txsK8);
    ASSERT_EQ(vout.size(), size_t(14));

    const std::byte expected_value[8] = {
        std::byte{0x58}, std::byte{0x0D}, std::byte{0x06}, std::byte{0x2A},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}
    };
    for (int i = 0; i < 8; ++i) EXPECT_EQ(vout[i], expected_value[i]);

    EXPECT_EQ(vout[8], std::byte{ 0x05 });
    const std::byte expected_script[5] = {
        std::byte{0x76}, std::byte{0xA9}, std::byte{0x14}, std::byte{0x88}, std::byte{0xAC}
    };
    for (int i = 0; i < 5; ++i) EXPECT_EQ(vout[9 + i], expected_script[i]);

    EXPECT_EQ(hex_lower(vout), "580d062a010000000576a91488ac");
}


TEST(Calculation_vout, K9) {
    std::vector<Transaction> txsK9 = {
        Transaction("0200000000011313",
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
            {}, /*fee=*/std::nullopt, std::optional<uint32_t>{0}, std::optional<uint64_t>{1000}),
        Transaction("0200000000011414",
            "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc",
            "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
            {}, std::optional<uint64_t>{7000}, std::optional<uint32_t>{0}, std::optional<uint64_t>{1000})
    };

    std::vector<std::byte> vout = BitcoinMainFunctions::calcVout("76a91488ac", 100000, txsK9);
    ASSERT_EQ(vout.size(), size_t(14));

    const std::byte expected_value[8] = {
        std::byte{0x58}, std::byte{0x0D}, std::byte{0x06}, std::byte{0x2A},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}
    };
    for (int i = 0; i < 8; ++i) EXPECT_EQ(vout[i], expected_value[i]);

    EXPECT_EQ(vout[8], std::byte{ 0x05 });
    const std::byte expected_script[5] = {
        std::byte{0x76}, std::byte{0xA9}, std::byte{0x14}, std::byte{0x88}, std::byte{0xAC}
    };
    for (int i = 0; i < 5; ++i) EXPECT_EQ(vout[9 + i], expected_script[i]);

    EXPECT_EQ(hex_lower(vout), "580d062a010000000576a91488ac");
}


TEST(Calculation_vout, K10) {
    std::vector<Transaction> txsK10 = {
        Transaction("0200000000011515",
            "1111111111111111111111111111111111111111111111111111111111111111",
            "2222222222222222222222222222222222222222222222222222222222222222",
            {}, std::optional<uint64_t>{0}, std::optional<uint32_t>{0}, std::optional<uint64_t>{1000})
    };

    std::vector<std::byte> vout = BitcoinMainFunctions::calcVout("76a91488ac", 100000, txsK10);
    ASSERT_EQ(vout.size(), size_t(14));

    const std::byte expected_value[8] = {
        std::byte{0x00}, std::byte{0xF2}, std::byte{0x05}, std::byte{0x2A},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}
    };
    for (int i = 0; i < 8; ++i) EXPECT_EQ(vout[i], expected_value[i]);

    EXPECT_EQ(vout[8], std::byte{ 0x05 });
    const std::byte expected_script[5] = {
        std::byte{0x76}, std::byte{0xA9}, std::byte{0x14}, std::byte{0x88}, std::byte{0xAC}
    };
    for (int i = 0; i < 5; ++i) EXPECT_EQ(vout[9 + i], expected_script[i]);

    EXPECT_EQ(hex_lower(vout), "00f2052a010000000576a91488ac");
}


TEST(Calculation_vout, K11) {
    std::vector<Transaction> txsK11 = {
        Transaction("0200000000011616",
            "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
            "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
            {}, std::optional<uint64_t>{4'000'000'000'000ULL}, std::optional<uint32_t>{0}, std::optional<uint64_t>{10'000})
    };

    std::vector<std::byte> vout = BitcoinMainFunctions::calcVout("76a91488ac", 100000, txsK11);
    ASSERT_EQ(vout.size(), size_t(14));

    const std::byte expected_value[8] = {
        std::byte{0x00}, std::byte{0x32}, std::byte{0x9A}, std::byte{0x7C},
        std::byte{0xA4}, std::byte{0x03}, std::byte{0x00}, std::byte{0x00}
    };
    for (int i = 0; i < 8; ++i) EXPECT_EQ(vout[i], expected_value[i]);

    EXPECT_EQ(vout[8], std::byte{ 0x05 });
    const std::byte expected_script[5] = {
        std::byte{0x76}, std::byte{0xA9}, std::byte{0x14}, std::byte{0x88}, std::byte{0xAC}
    };
    for (int i = 0; i < 5; ++i) EXPECT_EQ(vout[9 + i], expected_script[i]);

    EXPECT_EQ(hex_lower(vout), "00329a7ca40300000576a91488ac");
}

