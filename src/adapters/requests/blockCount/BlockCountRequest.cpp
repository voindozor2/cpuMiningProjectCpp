#include "BlockCountRequest.h"
#include <json/json.h>

BlockCountRequest::BlockCountRequest()
{
	jsonrpc = "1.0";
	id = "curltext";
	method = "getblockcount";
	params = Json::Value(Json::arrayValue);
}

std::string BlockCountRequest::toString() const
{
    Json::Value json;
    json["jsonrpc"] = jsonrpc;
    json["id"] = id;
    json["method"] = method;
    json["params"] = params;

    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, json);
}