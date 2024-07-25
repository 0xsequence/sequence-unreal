// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Sequence/DelayedEncodingBP.h"
#include "Indexer/IndexerSupport.h"
#include "Sequence/DelayedEncodingArgsBP.h"

void UDelayedEncodingBP::SetAbi(const FString& AbiIn)
{
	this->Abi = AbiIn;
}

void UDelayedEncodingBP::SetArgs(UDelayedEncodingArgsBP* ArgsIn)
{
	this->Args = ArgsIn;
}

void UDelayedEncodingBP::SetFunc(const FString& FuncIn)
{
	this->Func = FuncIn;
}

FString UDelayedEncodingBP::GetJsonString() const
{
	const FString Json = "{\"abi\":\"" + Abi + "\",\"args\":" + this->Args->GetJsonString() + ",\"func\":\"" + Func + "\"}";
	return UIndexerSupport::SimplifyStringParsable(Json);
}
