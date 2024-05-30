// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "Templates/ValueOrError.h"
#include "Containers/UnrealString.h"

enum EErrorType
{
	NotFound,
	ResponseParseError,
	EmptyResponse,
	UnsupportedMethodOnChain,
	RequestFail,
	RequestTimeExceeded,
	TestFail,
};

class SEQUENCEPLUGIN_API FSequenceError
{
public:
	FSequenceError(EErrorType Type, FString Message);
	FString Message;
	EErrorType Type;
};

template<typename T> using TResult = TValueOrError<T, FSequenceError>;
