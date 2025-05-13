// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Sequence/DelayedEncodingArgsBP.h"
#include "Util/SequenceSupport.h"

FString UDelayedEncodingArgsBP::GetJsonString()
{
	return "";
}

void UDelayedEncodingArrayArgsBP::AddStringArg(const FString& ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueString(*ArgIn));
	this->JsonArrayArg.Add(Value);
}

void UDelayedEncodingArrayArgsBP::AddBoolArg(const bool ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueBoolean(ArgIn));
	this->JsonArrayArg.Add(Value);
}

void UDelayedEncodingArrayArgsBP::AddFloatArg(const float ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueNumber(ArgIn));
	this->JsonArrayArg.Add(Value);
}

void UDelayedEncodingArrayArgsBP::AddDoubleArg(const double ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueNumber(ArgIn));
	this->JsonArrayArg.Add(Value);
}

void UDelayedEncodingArrayArgsBP::AddInt32Arg(const int32 ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueNumber(ArgIn));
	this->JsonArrayArg.Add(Value);
}

void UDelayedEncodingArrayArgsBP::AddInt64Arg(const int64 ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueNumber(ArgIn));
	this->JsonArrayArg.Add(Value);
}

void UDelayedEncodingArrayArgsBP::AddArrayArg(UDelayedEncodingArrayArgsBP* ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueArray(ArgIn->GetJson()));
	this->JsonArrayArg.Add(Value);
}

FString UDelayedEncodingArrayArgsBP::GetJsonString()
{
	return USequenceSupport::JsonValueListToParsableString(this->JsonArrayArg);
}

TArray<TSharedPtr<FJsonValue>> UDelayedEncodingArrayArgsBP::GetJson()
{
	return this->JsonArrayArg;
}
