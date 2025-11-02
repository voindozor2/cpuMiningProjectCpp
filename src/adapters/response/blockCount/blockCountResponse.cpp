#include "blockCountResponse.h"
#include <json/json.h>

BlockCountResponse::BlockCountResponse()
{
	count = 0;
}

BlockCountResponse::BlockCountResponse(const std::string& json)
{
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream iss(json);
    if (!Json::parseFromStream(builder, iss, &root, &errs)) {
        throw std::runtime_error("Failed to parse JSON: " + errs);
    }
    count = root["result"].asInt();
}

std::string BlockCountResponse::toString() const
{
    Json::Value json;
    json["blockCount"] = count;

    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, json);
}
