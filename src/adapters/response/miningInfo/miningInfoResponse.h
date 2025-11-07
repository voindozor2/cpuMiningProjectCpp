#pragma once
#include <string>
#include "../../../abstractions/serializedToString.h"

class MiningInfoResponse : public SerializedToString
{
public:
	int blocks;
	int currentBlockWeigth;
	int currentBlockTx;
	double difficulty;
	double networkHashps;
	int pooledTx;

	MiningInfoResponse();
	MiningInfoResponse(const std::string& json);
	~MiningInfoResponse() = default;
	std::string toString() const override;
};