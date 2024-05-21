// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Util/Async.h"
#include "Types/BinaryData.h"
#include "Types/Header.h"
#include "Dom/JsonObject.h"
#include "Types/TransactionReceipt.h"
#include "Eth/EthTransaction.h"
#include "RPCCaller.h"
#include "Types/ContractCall.h"
#include "Provider.generated.h"

struct FContractCall;

UENUM()
enum EBlockTag
{
	ELatest UMETA(DisplayName = "latest"),
	EEarliest UMETA(DisplayName = "earliest"),
	EPending UMETA(DisplayName = "pending"),
	ESafe UMETA(DisplayName = "safe"),
	EFinalized UMETA(DisplayName = "finalized"),
};

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UProvider : public URPCCaller
{
	GENERATED_BODY()
private:
	FString Url;

//helpers
	void BlockByNumberHelper(FString Number, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);
	void HeaderByNumberHelper(FString Number, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure);
	void NonceAtHelper(FString Number, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure);
	void CallHelper(FContractCall ContractCall, FString Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);
	void Init(const FString& UrlIn);
public:
	static UProvider* Make(const FString& UrlIn);
	void UpdateUrl(const FString& UrlIn);
	void BlockByNumber(uint64 Number, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);
	void BlockByNumber(EBlockTag Tag, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);
	void BlockByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);
	void BlockNumber(TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure);

	void HeaderByNumber(uint64 Id, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure);
	void HeaderByNumber(EBlockTag Tag, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure);
	void HeaderByHash(FHash256 Hash, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure);
	
	void TransactionByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);
	void TransactionCount(FAddress Addr, uint64 Number, TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure);
	void TransactionCount(FAddress Addr, EBlockTag Tag, TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure);
	void TransactionReceipt(FHash256 Hash, TSuccessCallback<FTransactionReceipt> OnSuccess, FFailureCallback OnFailure);

	void GetGasPrice(TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);
	void EstimateContractCallGas(FContractCall ContractCall, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);
	void EstimateDeploymentGas(FAddress From, FString Bytecode, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);

	void DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallback<FAddress> OnSuccess, FFailureCallback OnFailure);
	void DeployContractWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallbackTuple<FAddress, FUnsizedData> OnSuccess, FFailureCallback OnFailure);
	
	void NonceAt(uint64 Number, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure);
	void NonceAt(EBlockTag Tag, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure);
	void SendRawTransaction(FString Data, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);
	
	void ChainId(TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure);

	void Call(FContractCall ContractCall, uint64 Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);
	void Call(FContractCall ContractCall, EBlockTag Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);
	void NonViewCall(FEthTransaction transaction, FPrivateKey PrivateKey, int ChainID, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);
};