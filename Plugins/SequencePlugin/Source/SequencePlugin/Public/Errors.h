#pragma once
#include "Http.h"
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
	TimeMismatch,
	FailedToParseIntentTime,
	InvalidArgument,
	InsufficientBalance
};

class SEQUENCEPLUGIN_API FSequenceError
{
public:
	FSequenceError(EErrorType Type, const FString& Message);
	FSequenceError(EErrorType Type, const FHttpResponsePtr& Response, const FString& Message);
	FHttpResponsePtr Response;
	FString Message;
	EErrorType Type;
};

template<typename T> using TResult = TValueOrError<T, FSequenceError>;
