cmake_minimum_required(VERSION 4.0)
project(cpuMiningProject)

set(CMAKE_CXX_STANDARD 26)
add_executable(cpuMiningProject main.cpp)
set(VCPKG_TARGET_TRIPLET x64-mingw-dynamic)
find_package(protobuf CONFIG REQUIRED)
target_link_libraries(cpuMiningProject PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

find_package(gRPC CONFIG REQUIRED)
target_link_libraries(cpuMiningProject PRIVATE gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts)

find_package(Boost REQUIRED COMPONENTS optional)
target_link_libraries(cpuMiningProject PRIVATE Boost::optional)

find_package(Boost REQUIRED COMPONENTS endian)
target_link_libraries(cpuMiningProject PRIVATE Boost::endian)