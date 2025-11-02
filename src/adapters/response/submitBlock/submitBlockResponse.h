#pragma once
#include <string>
#include "../../../abstractions/serializedToString.h"

class SubmitBlockResponse : SerializedToString
{
public:
	std::string result;
	bool isSubmit;

	SubmitBlockResponse();
	SubmitBlockResponse(const std::string& json);
	~SubmitBlockResponse() = default;
	std::string toString() const override;
};