#include <iostream>
#include <cpr/cpr.h>
#include <json/json.h>
#include "src/adapters/bitcoinAdapter.h"
#include "src/adapters/requests/blockCount/BlockCountRequest.h"
#include "src/adapters/requests/blockTemplate/BlockTemplateRequest.h"
#include "src/adapters/requests/submitBlock/SubmitBlockRequest.h"
#include "src/adapters/requests/miningInfo/MiningInfoRequest.h"

#include "src/adapters/response/miningInfo/miningInfoResponse.h"
#include "src/adapters/response/submitBlock/submitBlockResponse.h"
#include "src/adapters/response/blockCount/blockCountResponse.h"
#include "src/adapters/response/blockTemplate/BlockTemplateResponse.h"


struct BlockchainInfo {
    std::string chain;
    int blocks;
    int headers;
    std::string bestblockhash;
    double difficulty;
};

struct RpcRequest {
    std::string jsonrpc;
    std::string id;
    std::string method;
    Json::Value params; // params может быть массивом или объектом
};

std::string to_json(const RpcRequest& req) {
    Json::Value j;
    j["jsonrpc"] = req.jsonrpc;
    j["id"] = req.id;
    j["method"] = req.method;
    j["params"] = req.params;

    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, j);
}

BlockchainInfo parse_blockchaininfo(const std::string& json_text) {
    BlockchainInfo info;
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream iss(json_text);
    if (!Json::parseFromStream(builder, iss, &root, &errs)) {
        throw std::runtime_error("Failed to parse JSON: " + errs);
    }

    const auto& result = root["result"];
    info.chain = result["chain"].asString();
    info.blocks = result["blocks"].asInt();
    info.headers = result["headers"].asInt();
    info.bestblockhash = result["bestblockhash"].asString();
    info.difficulty = result["difficulty"].asDouble();
    return info;
}

int main() {
    bitcoinAdapter adapter("127.0.0.1", "18332", "bitcoinrpc", "MyStrongTestPassword123");
    BlockCountRequest req;
    BlockTemplateRequest req2;
    MiningInfoRequest req3;
    std::string blockData = "00000020b1bcc6c8ca8aca6938ccbf75ecfb98126dabb724e4016704e56d4285f324dd4f5ad9fb1ac33413b4fe40a6d85112b37df85a5c0b4060901485b55612dd104e97c5870769ffff7f200400000001020000000001010000000000000000000000000000000000000000000000000000000000000000ffffffff03016600ffffffff0200f2052a010000001600144fa44aa78a68ceb98e8b0e89f6d521bfcbc2483c0000000000000000266a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf90120000000000000000000000000000000000000000000000000000000000000000000000000";

    
    std::cout << "BlockCountResponse" << std::endl;
	BlockCountResponse blockCountResponse = adapter.getBlockCount();
    std::cout << blockCountResponse.toString() << std::endl;

    std::cout << "BlockTemplateResponse" << std::endl;
    std::string response2 = adapter.send(&req2);
	BlockTemplateResponse blockTemplateResponse(response2);
    std::cout << blockTemplateResponse.toString() << std::endl;

    std::cout << "MiningInfoResponse" << std::endl;
   	MiningInfoResponse miningInfoResponse = adapter.getMiningInfo();
    std::cout << miningInfoResponse.toString() << std::endl;

    std::cout << "SubmitBlockResponse" << std::endl;
	SubmitBlockResponse submitBlockResponse = adapter.submitBlock(blockData);
    std::cout << submitBlockResponse.toString() << std::endl;
    return 0;
}


