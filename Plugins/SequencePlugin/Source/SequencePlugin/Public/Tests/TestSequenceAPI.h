#pragma once
#include "Errors.h"

class SequenceAPITest
{
public:
	static void RegisterSession(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void SignMessage(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void ListSessions(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void SendTransaction(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void CloseSession(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
};
