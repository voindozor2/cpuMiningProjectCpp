#pragma once
#include "../../../abstractions/serializedToString.h"
#include <json/json.h>

class SubmitBlockRequest : public SerializedToString
{
public:
	SubmitBlockRequest(std::string hexData);
	~SubmitBlockRequest() = default;
	std::string toString() const override;

private:
	std::string hexData;
	std::string jsonrpc;
	std::string id;
	std::string method;
	Json::Value params; // params может быть массивом или объектом
};