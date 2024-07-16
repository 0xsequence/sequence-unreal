// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Sequence/DelayedEncodingBP.h"

#include "Indexer/IndexerSupport.h"
#include "Sequence/DelayedEncodingArgsBP.h"

FString FDelayedEncoding::GetJsonString() const
{
	FString JsonList;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonList);
	FJsonSerializer::Serialize(Args, Writer);

	const FString Json = "{\"abi\":\"" + Abi + "\",\"args\":" + JsonList + ",\"func\":\"" + Func + "\"}";
	return UIndexerSupport::SimplifyStringParsable(Json);
}

void UDelayedEncodingBP::SetAbi(const FString& Abi)
{
	DlyEncObject.Abi = Abi;
}

void UDelayedEncodingBP::SetArgs(UDelayedEncodingArgsBP* ArgsIn)
{
	DlyEncObject.Args = ArgsIn->GetArgs();
}

void UDelayedEncodingBP::SetFunc(const FString& Func)
{
	DlyEncObject.Func = Func;
}

FString UDelayedEncodingBP::GetJsonString() const
{
	return DlyEncObject.GetJsonString();
}

FDelayedEncoding UDelayedEncodingBP::GetDelayedEncoding()
{
	return DlyEncObject;
}
