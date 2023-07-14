// Fill out your copyright notice in the Description page of Project Settings.



#pragma once

#include "CoreMinimal.h"
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
	void SendRPC(FString Content, TFunction<void (FString)> OnSuccess, TFunction<void (FHttpResponsePtr)> OnFailure);

	template<typename T>
	void SendRPCAndExtract(FString Content, TFunction<void (T)> OnSuccess, TFunction<TResult<T> (FString)> Extractor, TFunction<void (FHttpResponsePtr)> OnFailure);
	
	static FJsonBuilder RPCBuilder(FString MethodName);

//helpers
	TFuture<TResult<TSharedPtr<FJsonObject>>> GetBlockByNumberHelper(FString Number);
	TFuture<TResult<uint64>> TransactionCountHelper(FAddress Address, FString Number);
	TResult<FHeader> HeaderByNumberHelperSynchronous(FString Number);
	TFuture<TResult<FHeader>> HeaderByNumberHelper(FString Number);
	TResult<FBlockNonce> NonceAtHelperSynchronous(FString Number);
	TFuture<TResult<FBlockNonce>> NonceAtHelper(FString Number);
	TFuture<TResult<FNonUniformData>> CallHelper(FContractCall ContractCall, FString Number);
	TResult<FNonUniformData> CallHelperSynchronous(FContractCall ContractCall, FString Number);
	TResult<FHeader> HeaderByHashSynchronous(FHash256 Hash);
	TResult<FNonUniformData> GetGasPriceSynchronous();
	TResult<FNonUniformData> EstimateContractCallGasSynchronous(FContractCall ContractCall);
	TResult<FNonUniformData> EstimateDeploymentGasSynchronous(FAddress from, FString Bytecode);
	FAddress DeployContractSynchronous(FString Bytecode, FPrivateKey PrivKey, int64 ChainId);
	FAddress DeployContractSynchronousWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, FNonUniformData* TransactionHash);
	TResult<FTransactionReceipt> TransactionReceiptSynchronous(FHash256 Hash);
	TResult<uint64> ChainIdSynchronous();
	TResult<FNonUniformData> SendRawTransactionSynchronous(FString data);

public:
	Provider(FString Url);
	TFuture<TResult<TSharedPtr<FJsonObject>>> BlockByNumber(uint64 Number);
	TFuture<TResult<TSharedPtr<FJsonObject>>> BlockByNumber(EBlockTag Tag);
	void BlockByHash(FHash256 Hash, TFunction<void (TSharedPtr<FJsonObject>)> OnSuccess, TFunction<void (FHttpResponsePtr)> OnFailure);
	void BlockNumber(TFunction<void (uint64)> OnSuccess, TFunction<void (FHttpResponsePtr)> OnFailure);

	TFuture<TResult<FHeader>> HeaderByNumber(uint64 Id);
	TFuture<TResult<FHeader>> HeaderByNumber(EBlockTag Tag);
	TFuture<TResult<FHeader>> HeaderByHash(FHash256 Hash);
	
	TFuture<TResult<TSharedPtr<FJsonObject>>> TransactionByHash(FHash256 Hash);
	TFuture<TResult<uint64>> TransactionCount(FAddress Addr, uint64 Number);
	TFuture<TResult<uint64>> TransactionCount(FAddress Addr, EBlockTag Tag);
	TFuture<TResult<FTransactionReceipt>> TransactionReceipt(FHash256 Hash);

	TFuture<TResult<FNonUniformData>> GetGasPrice();
	TFuture<TResult<FNonUniformData>> EstimateContractCallGas(FContractCall ContractCall);
	TFuture<TResult<FNonUniformData>> EstimateDeploymentGas(FAddress from, FString Bytecode);

	TFuture<FAddress> DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId);
	TFuture<FAddress> DeployContractWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, FNonUniformData* TransactionHash);
	
	TFuture<TResult<FBlockNonce>> NonceAt(uint64 Number);
	TFuture<TResult<FBlockNonce>> NonceAt(EBlockTag Tag);
	TFuture<TResult<FNonUniformData>> SendRawTransaction(FString data);
	
	TFuture<TResult<uint64>> ChainId();

	TFuture<TResult<FNonUniformData>> Call(FContractCall ContractCall, uint64 Number);
	TFuture<TResult<FNonUniformData>> Call(FContractCall ContractCall, EBlockTag Number);
	TFuture<TResult<FNonUniformData>> NonViewCall(FEthTransaction transaction, FPrivateKey PrivateKey, int ChainID);

};

template <typename T>
void Provider::SendRPCAndExtract(FString Content, TFunction<void(T)> OnSuccess,
	TFunction<TResult<T>(FString)> Extractor, TFunction<void(FHttpResponsePtr)> OnFailure)
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


