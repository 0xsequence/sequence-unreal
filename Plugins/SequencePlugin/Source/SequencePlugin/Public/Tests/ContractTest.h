#pragma once
#include "Types/BinaryData.h"
#include "Provider.h"

class ContractTest
{
public:
	static void NonPayable(FAddress ContractAddress4, FString Url, FAddress Address2, FPrivateKey PrivateKey2, TFunction<void (FString)> OnSuccess, TFunction<void (FString, SequenceError)> OnFailure);
	static void GetBalance(const FAddress To, TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure);
	static void CallMethods(FAddress ContractAddress4, FString Url, FAddress Address2, FPrivateKey PrivateKey2, TFunction<void (FString)> OnSuccess, TFunction<void (FString, SequenceError)> OnFailure);
	static void DeployContract4(const FString Url, const FString VMByteCode, const FFailureCallback GenericFailure, const TFunction<void (FString)> OnSuccess, TFunction<void (FString, SequenceError)> OnFailure, FPrivateKey PrivateKey2, FAddress Address2);
	static void DeployContract3(const FString Url, const FString VMByteCode, const FFailureCallback GenericFailure, const TFunction<void (FString)> OnSuccess, TFunction<void (FString, SequenceError)> OnFailure, FPrivateKey PrivateKey1, FAddress Address1, FPrivateKey PrivateKey2, FAddress Address2);
	static void DeployContract2(FString Url, FString LongByteCode, FString VMByteCode, FFailureCallback GenericFailure, TFunction<void (FString)> OnSuccess, TFunction<void (FString, SequenceError)> OnFailure, FPrivateKey PrivateKey1, FAddress Address1, FPrivateKey PrivateKey2, FAddress Address2);
	static void DeployContracts(FString Url,  FString LongByteCode, FString SmallByteCode, FString VMByteCode, FFailureCallback GenericFailure, TFunction<void (FString)> OnSuccess, TFunction<void (FString, SequenceError)> OnFailure, FPrivateKey PrivateKey1, FAddress Address1, FPrivateKey PrivateKey2, FAddress Address2);
public:
	static void RunTest(TFunction<void (FString)> OnSuccess, TFunction<void (FString, SequenceError)> OnFailure);
};
