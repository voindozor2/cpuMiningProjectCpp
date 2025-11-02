#include "SubmitBlockRequest.h"

SubmitBlockRequest::SubmitBlockRequest(std::string hexData)
{
	jsonrpc = "1.0";
	id = "curltext";
	method = "submitblock";
	

	Json::Value params(Json::arrayValue);
	params.append(hexData);

	this->params = params;
}

std::string SubmitBlockRequest::toString() const
{
	Json::Value json;
	json["jsonrpc"] = jsonrpc;
	json["id"] = id;
	json["method"] = method;
	json["params"] = params;

	Json::StreamWriterBuilder writer;
	return Json::writeString(writer, json);
}
