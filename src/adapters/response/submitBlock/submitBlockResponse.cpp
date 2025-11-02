#include "submitBlockResponse.h"
#include <json/json.h>

SubmitBlockResponse::SubmitBlockResponse()
{
	this->result = "";
	this->isSubmit = false;
}

SubmitBlockResponse::SubmitBlockResponse(const std::string& json)
{
	Json::Value root;
	Json::CharReaderBuilder builder;
	std::string errs;
	std::istringstream iss(json);
	if (!Json::parseFromStream(builder, iss, &root, &errs)) {
		throw std::runtime_error("Failed to parse JSON: " + errs);
	}
	result = root["result"].asString();

	if(result.empty() && root["error"].empty()) {
		isSubmit = true;
	} else {
		isSubmit = false;
	}
}

std::string SubmitBlockResponse::toString() const
{
	Json::Value json;
	json["result"] = result;
	json["isSubmit"] = isSubmit;

	Json::StreamWriterBuilder writer;
	return Json::writeString(writer, json);
}
