#pragma once

enum ErrorType
{
	ErrNotFound,
	ErrEmptyResponse,
	ErrUnsupportedMethodOnChain,
	ErrRequestFail,
};

class SequenceError
{
public:
	SequenceError(ErrorType Type, FString Message);
	FString Message;
	ErrorType Type;
};
