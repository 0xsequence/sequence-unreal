// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Sequence/DelayedEncodingArgsBP.h"
#include "Util/SequenceSupport.h"

FString UDelayedEncodingArgsBP::GetJsonString()
{
	return "";
}

UDelayedEncodingObjectArgsBP::UDelayedEncodingObjectArgsBP()
{
	this->JsonObjectArg = MakeShareable(new FJsonObject);
}

void UDelayedEncodingObjectArgsBP::AddStringArg(const FString& ArgName, const FString& ArgIn)
{
	this->JsonObjectArg.Get()->SetStringField(ArgName,*ArgIn);
}

void UDelayedEncodingObjectArgsBP::AddBoolArg(const FString& ArgName, const bool ArgIn)
{
	this->JsonObjectArg.Get()->SetBoolField(ArgName, ArgIn);
}

void UDelayedEncodingObjectArgsBP::AddFloatArg(const FString& ArgName, const float ArgIn)
{
	this->JsonObjectArg.Get()->SetNumberField(ArgName, ArgIn);
}

void UDelayedEncodingObjectArgsBP::AddDoubleArg(const FString& ArgName, const double ArgIn)
{
	this->JsonObjectArg.Get()->SetNumberField(ArgName, ArgIn);
}

void UDelayedEncodingObjectArgsBP::AddInt32Arg(const FString& ArgName, const int32 ArgIn)
{
	this->JsonObjectArg.Get()->SetNumberField(ArgName, ArgIn);
}

void UDelayedEncodingObjectArgsBP::AddInt64Arg(const FString& ArgName, const int64 ArgIn)
{
	this->JsonObjectArg.Get()->SetNumberField(ArgName, ArgIn);
}

void UDelayedEncodingObjectArgsBP::AddObjectArg(const FString& ArgName, UDelayedEncodingObjectArgsBP* ArgIn)
{
	this->JsonObjectArg.Get()->SetObjectField(ArgName, ArgIn->GetJson());
}

void UDelayedEncodingObjectArgsBP::AddArrayArg(const FString& ArgName, UDelayedEncodingArrayArgsBP* ArgIn)
{
	this->JsonObjectArg.Get()->SetArrayField(ArgName, ArgIn->GetJson());
}

FString UDelayedEncodingObjectArgsBP::GetJsonString()
{
	return USequenceSupport::JsonToParsableString(this->JsonObjectArg);
}

TSharedPtr<FJsonObject> UDelayedEncodingObjectArgsBP::GetJson()
{
	return this->JsonObjectArg;
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

void UDelayedEncodingArrayArgsBP::AddObjectArg(UDelayedEncodingObjectArgsBP* ArgIn)
{
	const TSharedPtr<FJsonValue> Value = MakeShareable(new FJsonValueObject(ArgIn->GetJson()));
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
