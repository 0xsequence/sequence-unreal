// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "Templates/ValueOrError.h"

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

class FSequenceError
{
public:
	FSequenceError(EErrorType Type, FString Message);
	FString Message;
	EErrorType Type;
};

template<typename T> using TResult = TValueOrError<T, FSequenceError>;
