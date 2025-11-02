#include "MiningInfoRequest.h"

MiningInfoRequest::MiningInfoRequest()
{
	jsonrpc = "1.0";
	id = "curltext";
	method = "getmininginfo";
	params = Json::Value(Json::arrayValue);
}

std::string MiningInfoRequest::toString() const
{
    Json::Value json;
    json["jsonrpc"] = jsonrpc;
    json["id"] = id;
    json["method"] = method;
    json["params"] = params;

    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, json);
}