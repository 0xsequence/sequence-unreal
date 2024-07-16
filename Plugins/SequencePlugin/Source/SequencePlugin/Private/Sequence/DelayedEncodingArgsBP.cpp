// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Sequence/DelayedEncodingArgsBP.h"

#include "Indexer/IndexerSupport.h"
#include "Sequence/DelayedEncodingBP.h"

void FDelayedEncodingArgs::AddStringArg(const FString& ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueString(ArgIn));
	Args.Add(Value);
}

void FDelayedEncodingArgs::AddBoolArg(const bool ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueBoolean(ArgIn));
	Args.Add(Value);
}

void FDelayedEncodingArgs::AddFloatArg(const float ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueNumber(ArgIn));
	Args.Add(Value);
}

void FDelayedEncodingArgs::AddDoubleArg(const double ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueNumber(ArgIn));
	Args.Add(Value);
}

void FDelayedEncodingArgs::AddInt32Arg(const int32 ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueNumber(ArgIn));
	Args.Add(Value);
}

void FDelayedEncodingArgs::AddInt64Arg(const int64 ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueNumber(ArgIn));
	Args.Add(Value);
}

void FDelayedEncodingArgs::AddDelayedEncodingArg(const FDelayedEncoding& Obj)
{
	const TSharedPtr<FJsonObject> JsonObject = UIndexerSupport::JsonStringToObject(Obj.GetJsonString());
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueObject(JsonObject));
	Args.Add(Value);
}

TArray<TSharedPtr<FJsonValue>> FDelayedEncodingArgs::GetArgs()
{
	return Args;
}

void UDelayedEncodingArgsBP::AddStringArg(const FString& ArgIn)
{
	Args.AddStringArg(ArgIn);
}

void UDelayedEncodingArgsBP::AddBoolArg(const bool ArgIn)
{
	Args.AddBoolArg(ArgIn);
}

void UDelayedEncodingArgsBP::AddFloatArg(const float ArgIn)
{
	Args.AddFloatArg(ArgIn);
}

void UDelayedEncodingArgsBP::AddDoubleArg(const double ArgIn)
{
	Args.AddDoubleArg(ArgIn);
}

void UDelayedEncodingArgsBP::AddInt32Arg(const int32 ArgIn)
{
	Args.AddInt32Arg(ArgIn);
}

void UDelayedEncodingArgsBP::AddInt64Arg(const int64 ArgIn)
{
	Args.AddInt64Arg(ArgIn);
}

void UDelayedEncodingArgsBP::AddDelayedEncodingArg(UDelayedEncodingBP* ArgIn)
{
	Args.AddDelayedEncodingArg(ArgIn->GetDelayedEncoding());
}

TArray<TSharedPtr<FJsonValue>> UDelayedEncodingArgsBP::GetArgs()
{
	return Args.GetArgs();
}
