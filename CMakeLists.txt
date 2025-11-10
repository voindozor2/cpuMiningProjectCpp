cmake_minimum_required(VERSION 4.0)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/Boris.DESKTOP-L651092/CLionProjects/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(cpuMiningProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
add_executable(cpuMiningProject main.cpp
    src/abstractions/serializedToString.h
    src/adapters/bitcoinAdapter.cpp
    src/adapters/requests/blockCount/BlockCountRequest.cpp
    src/adapters/requests/miningInfo/MiningInfoRequest.cpp
    src/adapters/requests/blockTemplate/BlockTemplateRequest.cpp
    src/adapters/requests/submitBlock/SubmitBlockRequest.cpp
    src/adapters/response/blockCount/blockCountResponse.cpp
    src/adapters/response/miningInfo/miningInfoResponse.cpp
    src/adapters/response/submitBlock/submitBlockResponse.cpp
    src/adapters/response/blockTemplate/Transaction.cpp
    src/adapters/response/blockTemplate/CoinBaseTxn.cpp
    src/adapters/response/blockTemplate/BlockTemplateResponse.cpp 
    src/utils/VarInt.cpp
    src/core/BitcoinMainFunctions.h
    src/core/BitcoinMainFunctions.cpp 
    src/adapters/requests/listunspent/ListUnspentRequest.cpp
    src/adapters/requests/listunspent/ListUnspentRequest.h
    src/adapters/response/listunspent/ListUnspentResponse.h
    src/adapters/response/listunspent/ListUnspentResponse.cpp
)

find_package(protobuf CONFIG REQUIRED)
target_link_libraries(cpuMiningProject PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

find_package(gRPC CONFIG REQUIRED)
target_link_libraries(cpuMiningProject PRIVATE gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts)

find_package(Boost REQUIRED COMPONENTS optional)
target_link_libraries(cpuMiningProject PRIVATE Boost::optional)

find_package(Boost REQUIRED COMPONENTS endian)
target_link_libraries(cpuMiningProject PRIVATE Boost::endian)

find_package(cpr REQUIRED)
target_link_libraries(cpuMiningProject PRIVATE cpr::cpr)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(cpuMiningProject PRIVATE jsoncpp_lib)

find_package(Boost REQUIRED COMPONENTS algorithm)
target_link_libraries(cpuMiningProject PRIVATE Boost::algorithm)

# === Тесты ===
include(CTest)                 # нужно для ctest/Test Explorer

if(BUILD_TESTING)
  find_package(GTest CONFIG REQUIRED)

  add_executable(unit_tests
    tests/test_main.cpp     # только тут кладём тестовый файл(ы)
    src/abstractions/serializedToString.h
    src/adapters/bitcoinAdapter.cpp
    src/adapters/requests/blockCount/BlockCountRequest.cpp
    src/adapters/requests/miningInfo/MiningInfoRequest.cpp
    src/adapters/requests/blockTemplate/BlockTemplateRequest.cpp
    src/adapters/requests/submitBlock/SubmitBlockRequest.cpp
    src/adapters/response/blockCount/blockCountResponse.cpp
    src/adapters/response/miningInfo/miningInfoResponse.cpp
    src/adapters/response/submitBlock/submitBlockResponse.cpp
    src/adapters/response/blockTemplate/Transaction.cpp
    src/adapters/response/blockTemplate/CoinBaseTxn.cpp
    src/adapters/response/blockTemplate/BlockTemplateResponse.cpp 
    src/utils/VarInt.cpp
    src/core/BitcoinMainFunctions.h
    src/core/BitcoinMainFunctions.cpp
    src/adapters/requests/listunspent/ListUnspentRequest.cpp
    src/adapters/requests/listunspent/ListUnspentRequest.h
    src/adapters/response/listunspent/ListUnspentResponse.h
    src/adapters/response/listunspent/ListUnspentResponse.cpp
  )
  target_link_libraries(unit_tests PRIVATE GTest::gtest GTest::gtest_main)
  target_link_libraries(unit_tests PRIVATE Boost::optional)
  target_link_libraries(unit_tests PRIVATE Boost::endian)
  target_link_libraries(unit_tests PRIVATE cpr::cpr)
  target_link_libraries(unit_tests PRIVATE jsoncpp_lib)
  target_link_libraries(unit_tests PRIVATE Boost::algorithm)
  target_link_libraries(unit_tests PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)
  target_link_libraries(unit_tests PRIVATE gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts)

  # чтобы VS Test Explorer видел и умел запускать/отлаживать поштучно
  include(GoogleTest)
  gtest_discover_tests(unit_tests
      DISCOVERY_MODE PRE_TEST           # надёжная модель обнаружения
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}  # по желанию
  )
endif()