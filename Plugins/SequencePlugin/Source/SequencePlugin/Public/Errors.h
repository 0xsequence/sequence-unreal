#pragma once


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
