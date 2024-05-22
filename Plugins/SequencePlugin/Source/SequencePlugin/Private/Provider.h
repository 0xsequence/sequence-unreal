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
#include "ProviderEnum.h"
#include "Provider.generated.h"

struct FContractCall;

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
	void BlockByNumberHelper(const FString& Number, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure);
	void HeaderByNumberHelper(const FString& Number, TSuccessCallback<FHeader> OnSuccess, const FFailureCallback& OnFailure);
	void NonceAtHelper(const FString& Number, TSuccessCallback<FBlockNonce> OnSuccess, const FFailureCallback& OnFailure);
	void CallHelper(FContractCall ContractCall, const FString& Number, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure);
	void Init(const FString& UrlIn);
public:
	static UProvider* Make(const FString& UrlIn);
	void UpdateUrl(const FString& UrlIn);
	void BlockByNumber(const uint64 Number, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure);
	void BlockByNumber(const EBlockTag Tag, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure);
	void BlockByHash(const FHash256& Hash, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure);
	void BlockNumber(const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure);

	void HeaderByNumber(const uint64 Id, const TSuccessCallback<FHeader>& OnSuccess, const FFailureCallback& OnFailure);
	void HeaderByNumber(const EBlockTag Tag, const TSuccessCallback<FHeader>& OnSuccess, const FFailureCallback& OnFailure);
	void HeaderByHash(const FHash256& Hash, TSuccessCallback<FHeader> OnSuccess, const FFailureCallback& OnFailure);
	
	void TransactionByHash(const FHash256& Hash, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure);
	void TransactionCount(const FAddress& Addr, const uint64 Number, const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure);
	void TransactionCount(const FAddress& Addr, const EBlockTag Tag, const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure);
	void TransactionReceipt(const FHash256& Hash, const TSuccessCallback<FTransactionReceipt>& OnSuccess, const FFailureCallback& OnFailure);

	void GetGasPrice(const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure);
	void EstimateContractCallGas(FContractCall ContractCall, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure);
	void EstimateDeploymentGas(const FAddress& From, const FString& Bytecode, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure);

	void DeployContract(const FString& Bytecode, const FPrivateKey& PrivKey, const int64 ChainId, const TSuccessCallback<FAddress>& OnSuccess, const FFailureCallback& OnFailure);
	void DeployContractWithHash(const FString& Bytecode, const FPrivateKey& PrivKey, const int64 ChainId, const TSuccessCallbackTuple<FAddress, FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure);
	
	void NonceAt(const uint64 Number, const TSuccessCallback<FBlockNonce>& OnSuccess, const FFailureCallback& OnFailure);
	void NonceAt(const EBlockTag Tag, const TSuccessCallback<FBlockNonce>& OnSuccess, const FFailureCallback& OnFailure);
	void SendRawTransaction(const FString& Data, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure);
	
	void ChainId(const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure);

	void Call(const FContractCall& ContractCall, const uint64 Number, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure);
	void Call(const FContractCall& ContractCall, const EBlockTag Number, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure);
	void NonViewCall(FEthTransaction Transaction, const FPrivateKey& PrivateKey, const int ChainID, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure);
};