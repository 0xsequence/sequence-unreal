// Fill out your copyright notice in the Description page of Project Settings.



#pragma once

#include "CoreMinimal.h"
#include "Async.h"
#include "Types/BinaryData.h"
#include "Errors.h"
#include "Types/Header.h"
#include "Http.h"
#include "JsonBuilder.h"
#include "Types/TransactionReceipt.h"
#include "Types/Types.h"
#include "EthTransaction.h"

struct FContractCall;

enum EBlockTag
{
	Latest,
	Earliest,
	Pending,
	Safe,
	Finalized
};

FString TagToString(EBlockTag Tag);

/**
 * 
 */
class Provider
{
	FString Url;
	TSharedPtr<FJsonObject> Parse(FString JsonRaw);
	TResult<TSharedPtr<FJsonObject>> ExtractJsonObjectResult(FString JsonRaw);
	TResult<FString> ExtractStringResult(FString JsonRaw);
	TResult<uint64> ExtractUIntResult(FString JsonRaw);
	void SendRPC(FString Content, TSuccessCallback<FString> OnSuccess, TFailureCallback OnFailure);

	template<typename T>
	void SendRPCAndExtract(FString Content, TSuccessCallback<T> OnSuccess, TFunction<TResult<T> (FString)> Extractor, TFailureCallback OnFailure);
	
	static FJsonBuilder RPCBuilder(FString MethodName);

//helpers
	void BlockByNumberHelper(FString Number, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, TFailureCallback OnFailure);
	void HeaderByNumberHelper(FString Number, TSuccessCallback<FHeader> OnSuccess, TFailureCallback OnFailure);
	void NonceAtHelper(FString Number, TSuccessCallback<FBlockNonce> OnSuccess, TFailureCallback OnFailure);
	void CallHelper(FContractCall ContractCall, FString Number, TSuccessCallback<FNonUniformData> OnSuccess, TFailureCallback OnFailure);

public:
	Provider(FString Url);
	void BlockByNumber(uint64 Number, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, TFailureCallback OnFailure);
	void BlockByNumber(EBlockTag Tag, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, TFailureCallback OnFailure);
	void BlockByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, TFailureCallback OnFailure);
	void BlockNumber(TSuccessCallback<uint64> OnSuccess, TFailureCallback OnFailure);

	void HeaderByNumber(uint64 Id, TSuccessCallback<FHeader> OnSuccess, TFailureCallback OnFailure);
	void HeaderByNumber(EBlockTag Tag, TSuccessCallback<FHeader> OnSuccess, TFailureCallback OnFailure);
	void HeaderByHash(FHash256 Hash, TSuccessCallback<FHeader> OnSuccess, TFailureCallback OnFailure);
	
	void TransactionByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, TFailureCallback OnFailure);
	void TransactionCount(FAddress Addr, uint64 Number, TSuccessCallback<uint64> OnSuccess, TFailureCallback OnFailure);
	void TransactionCount(FAddress Addr, EBlockTag Tag, TSuccessCallback<uint64> OnSuccess, TFailureCallback OnFailure);
	void TransactionReceipt(FHash256 Hash, TSuccessCallback<FTransactionReceipt> OnSuccess, TFailureCallback OnFailure);

	void GetGasPrice(TSuccessCallback<FNonUniformData>, TFailureCallback OnFailure);
	void EstimateContractCallGas(FContractCall ContractCall, TSuccessCallback<FNonUniformData>, TFailureCallback OnFailure);
	void EstimateDeploymentGas(FAddress From, FString Bytecode, TSuccessCallback<FNonUniformData>, TFailureCallback OnFailure);

	void DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallback<FAddress> OnSuccess, TFailureCallback OnFailure);
	void DeployContractWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, FNonUniformData* TransactionHash, TSuccessCallback<FAddress> OnSuccess, TFailureCallback OnFailure);
	
	void NonceAt(uint64 Number, TSuccessCallback<FBlockNonce> OnSuccess, TFailureCallback OnFailure);
	void NonceAt(EBlockTag Tag, TSuccessCallback<FBlockNonce> OnSuccess, TFailureCallback OnFailure);
	void SendRawTransaction(FString Data, TSuccessCallback<FNonUniformData> OnSuccess, TFailureCallback OnFailure);
	
	void ChainId(TSuccessCallback<uint64> OnSuccess, TFailureCallback OnFailure);

	void Call(FContractCall ContractCall, uint64 Number, TSuccessCallback<FNonUniformData> OnSuccess, TFailureCallback OnFailure);
	void Call(FContractCall ContractCall, EBlockTag Number, TSuccessCallback<FNonUniformData> OnSuccess, TFailureCallback OnFailure);
	void NonViewCall(FEthTransaction transaction, FPrivateKey PrivateKey, int ChainID, TSuccessCallback<FNonUniformData> OnSuccess, TFailureCallback OnFailure);

};

template<typename T> using Extractor = TFunction<TResult<T> (FString)>;

template <typename T>
void Provider::SendRPCAndExtract(FString Content, TSuccessCallback<T> OnSuccess,
	Extractor<T> Extractor, TFailureCallback OnFailure)
{
	SendRPC(Content, [&OnSuccess, &Extractor](FString Result)
	{
		TResult<T> Value = Extractor(Result);

		if(Value.HasValue())
		{
			OnSuccess(Value.GetValue());
		}
	}, OnFailure);
}


