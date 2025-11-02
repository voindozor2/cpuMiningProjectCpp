#pragma once
#include "../../../abstractions/serializedToString.h"
#include <json/json.h>

class BlockCountRequest : public SerializedToString
{
public:
	BlockCountRequest();
	~BlockCountRequest() = default;
	std::string toString() const override;

private:
	std::string jsonrpc;
	std::string id;
	std::string method;
	Json::Value params; // params может быть массивом или объектом
};