#include "ListUnspentRequest.h"
#include <json/json.h>

ListUnspent::ListUnspent()
{
	jsonrpc = "1.0";
	id = "curltext";
	method = "listunspent";
	params = Json::Value(Json::arrayValue);
}

std::string ListUnspent::toString() const
{
    Json::Value json;
    json["jsonrpc"] = jsonrpc;
    json["id"] = id;
    json["method"] = method;
    json["params"] = params;

    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, json);
}