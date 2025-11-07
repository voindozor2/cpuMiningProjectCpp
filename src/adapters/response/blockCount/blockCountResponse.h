#pragma once
#include <string>
#include "../../../abstractions/serializedToString.h"

class BlockCountResponse : public SerializedToString
{
public:
	int count;

	BlockCountResponse();
	~BlockCountResponse() = default;
	BlockCountResponse(const std::string& json);
	std::string toString() const override;
};