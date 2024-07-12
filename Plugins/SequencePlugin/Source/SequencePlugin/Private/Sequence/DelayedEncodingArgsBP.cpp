// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Sequence/DelayedEncodingArgsBP.h"
#include "Sequence/DelayedEncodingBP.h"

void FDelayedEncodingArgs::AddStringArg(const FString& ArgIn)
{
}

void FDelayedEncodingArgs::AddBoolArg(const bool ArgIn)
{
}

void FDelayedEncodingArgs::AddFloatArg(const float ArgIn)
{
}

void FDelayedEncodingArgs::AddInt32Arg(const int32 ArgIn)
{
}

void FDelayedEncodingArgs::AddInt64Arg(const int64 ArgIn)
{
}

void FDelayedEncodingArgs::AddDelayedEncodingArg(const FDelayedEncoding& Obj)
{
}

TArray<TSharedPtr<FJsonValue>> FDelayedEncodingArgs::GetArgs()
{
	return {};
}

void UDelayedEncodingArgsBP::AddStringArg(const FString& ArgIn)
{
}

void UDelayedEncodingArgsBP::AddBoolArg(const bool ArgIn)
{
}

void UDelayedEncodingArgsBP::AddFloatArg(const float ArgIn)
{
}

void UDelayedEncodingArgsBP::AddInt32Arg(const int32 ArgIn)
{
}

void UDelayedEncodingArgsBP::AddInt64Arg(const int64 ArgIn)
{
}

void UDelayedEncodingArgsBP::AddDelayedEncodingArg(UDelayedEncodingBP* ArgIn)
{
}

TArray<TSharedPtr<FJsonValue>> UDelayedEncodingArgsBP::GetArgs()
{
	return {};
}
