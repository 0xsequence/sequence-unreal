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
	void SendRPC(FString Content, SuccessCallback<FString> OnSuccess, FailureCallback OnFailure);

	template<typename T>
	void SendRPCAndExtract(FString Content, SuccessCallback<T> OnSuccess, TFunction<TResult<T> (FString)> Extractor, FailureCallback OnFailure);
	
	static FJsonBuilder RPCBuilder(FString MethodName);

//helpers
	void BlockByNumberHelper(FString Number, SuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FailureCallback OnFailure);
	void HeaderByNumberHelper(FString Number, SuccessCallback<FHeader> OnSuccess, FailureCallback OnFailure);
	void NonceAtHelper(FString Number, SuccessCallback<FBlockNonce> OnSuccess, FailureCallback OnFailure);
	void CallHelper(FContractCall ContractCall, FString Number, SuccessCallback<FNonUniformData> OnSuccess, FailureCallback OnFailure);
	FAddress DeployContractSynchronous(FString Bytecode, FPrivateKey PrivKey, int64 ChainId);
	FAddress DeployContractSynchronousWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, FNonUniformData* TransactionHash);

public:
	Provider(FString Url);
	void BlockByNumber(uint64 Number, SuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FailureCallback OnFailure);
	void BlockByNumber(EBlockTag Tag, SuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FailureCallback OnFailure);
	void BlockByHash(FHash256 Hash, SuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FailureCallback OnFailure);
	void BlockNumber(SuccessCallback<uint64> OnSuccess, FailureCallback OnFailure);

	void HeaderByNumber(uint64 Id, SuccessCallback<FHeader> OnSuccess, FailureCallback OnFailure);
	void HeaderByNumber(EBlockTag Tag, SuccessCallback<FHeader> OnSuccess, FailureCallback OnFailure);
	void HeaderByHash(FHash256 Hash, SuccessCallback<FHeader> OnSuccess, FailureCallback OnFailure);
	
	void TransactionByHash(FHash256 Hash, SuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FailureCallback OnFailure);
	void TransactionCount(FAddress Addr, uint64 Number, SuccessCallback<uint64> OnSuccess, FailureCallback OnFailure);
	void TransactionCount(FAddress Addr, EBlockTag Tag, SuccessCallback<uint64> OnSuccess, FailureCallback OnFailure);
	void TransactionReceipt(FHash256 Hash, SuccessCallback<FTransactionReceipt> OnSuccess, FailureCallback OnFailure);

	void GetGasPrice(SuccessCallback<FNonUniformData>, FailureCallback OnFailure);
	void EstimateContractCallGas(FContractCall ContractCall, SuccessCallback<FNonUniformData>, FailureCallback OnFailure);
	void EstimateDeploymentGas(FAddress From, FString Bytecode, SuccessCallback<FNonUniformData>, FailureCallback OnFailure);

	void DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, SuccessCallback<FAddress> OnSuccess, FailureCallback OnFailure);
	void DeployContractWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, FNonUniformData* TransactionHash, SuccessCallback<FAddress> OnSuccess, FailureCallback OnFailure);
	
	void NonceAt(uint64 Number, SuccessCallback<FBlockNonce> OnSuccess, FailureCallback OnFailure);
	void NonceAt(EBlockTag Tag, SuccessCallback<FBlockNonce> OnSuccess, FailureCallback OnFailure);
	void SendRawTransaction(FString Data, SuccessCallback<FNonUniformData> OnSuccess, FailureCallback OnFailure);
	
	void ChainId(SuccessCallback<uint64> OnSuccess, FailureCallback OnFailure);

	void Call(FContractCall ContractCall, uint64 Number, SuccessCallback<FNonUniformData> OnSuccess, FailureCallback OnFailure);
	void Call(FContractCall ContractCall, EBlockTag Number, SuccessCallback<FNonUniformData> OnSuccess, FailureCallback OnFailure);
	void NonViewCall(FEthTransaction transaction, FPrivateKey PrivateKey, int ChainID, SuccessCallback<FNonUniformData> OnSuccess, FailureCallback OnFailure);

};

template<typename T> using Extractor = TFunction<TResult<T> (FString)>;

template <typename T>
void Provider::SendRPCAndExtract(FString Content, SuccessCallback<T> OnSuccess,
	Extractor<T> Extractor, FailureCallback OnFailure)
{
	SendRPC(Content, [&OnSuccess, &Extractor](FString Result)
	{
		auto Value = Extractor(Result);

		if(Value.hasValue())
		{
			OnSuccess(Value.getValue());
		}
	}, OnFailure);
}


