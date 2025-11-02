#include "BlockTemplateRequest.h"
#include <json/json.h>
#pragma once

BlockTemplateRequest::BlockTemplateRequest()
{
	jsonrpc = "1.0";
	id = "curltext";
	method = "getblocktemplate";
    params = Json::Value(Json::arrayValue);
    Json::Value templateRequest(Json::objectValue);
    
    templateRequest["mode"] = "template";
    templateRequest["capabilities"] = Json::Value(Json::arrayValue);
    templateRequest["capabilities"].append("longpoll");
    templateRequest["capabilities"].append("coinbasevalue");
    templateRequest["capabilities"].append("workid");

    templateRequest["rules"]= Json::Value(Json::arrayValue);
    templateRequest["rules"].append("segwit");

    params.append(templateRequest);
}

std::string BlockTemplateRequest::toString() const
{
    Json::Value json;
    json["jsonrpc"] = jsonrpc;
    json["id"] = id;
    json["method"] = method;
    json["params"] = params;

    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, json);
}
