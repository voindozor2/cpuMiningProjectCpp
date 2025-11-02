#include <iostream>
#include <cpr/cpr.h>
#include <json/json.h>

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
    RpcRequest req;
    req.jsonrpc = "1.0";
    req.id = "curltext";
    req.method = "getblockchaininfo";
    req.params = Json::Value(Json::arrayValue); // пустой массив

    std::string json_body = to_json(req);

    std::cout << json_body << std::endl;

    auto response = cpr::Post(
        cpr::Url{"http://127.0.0.1:18443"},
        cpr::Authentication{"bitcoinrpc", "MyStrongTestPassword123",cpr::AuthMode::BASIC},
        //cpr::Body{R"({"jsonrpc":"1.0","id":"curltext","method":"getblockchaininfo","params":[]})"},
        cpr::Body{json_body},
        cpr::Header{{"content-type", "text/plain"}});

    
    BlockchainInfo info = parse_blockchaininfo(response.text);
    std::cout << info.chain << " | blocks: " << info.blocks << std::endl;

    //std::cout << response.text << std::endl;
    return 0;
}


