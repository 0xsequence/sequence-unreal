// Fill out your copyright notice in the Description page of Project Settings.



#pragma once

#include "CoreMinimal.h"
#include "Types/BinaryData.h"
#include "Errors.h"
#include "Types/Header.h"
#include "Http.h"
#include "JsonBuilder.h"
#include "Types/FTransactionReceipt.h"
#include "Types/Types.h"
#include "FEthTransaction.h"

struct ContractCall;

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
	TFuture<FString> SendRPC(FString Content);
	static FJsonBuilder RPCBuilder(FString MethodName);

//helpers
	TFuture<TResult<TSharedPtr<FJsonObject>>> GetBlockByNumberHelper(FString Number);
	TFuture<TResult<uint64>> TransactionCountHelper(FAddress Address, FString Number);
	TResult<FHeader> HeaderByNumberHelperSynchronous(FString Number);
	TFuture<TResult<FHeader>> HeaderByNumberHelper(FString Number);
	TResult<FBlockNonce> NonceAtHelperSynchronous(FString Number);
	TFuture<TResult<FBlockNonce>> NonceAtHelper(FString Number);
	TFuture<TResult<FNonUniformData>> CallHelper(ContractCall ContractCall, FString Number);
	TResult<FHeader> HeaderByHashSynchronous(FHash256 Hash);
	TResult<FNonUniformData> GetGasPriceSynchronous();
	TResult<FNonUniformData> EstimateContractCallGasSynchronous(ContractCall ContractCall);
	TResult<FNonUniformData> EstimateDeploymentGasSynchronous(FAddress from, FString Bytecode);
	
public:
	Provider(FString Url);
	TFuture<TResult<TSharedPtr<FJsonObject>>> BlockByNumber(uint64 Number);
	TFuture<TResult<TSharedPtr<FJsonObject>>> BlockByNumber(EBlockTag Tag);
	TFuture<TResult<TSharedPtr<FJsonObject>>> BlockByHash(FHash256 Hash);
	TFuture<TResult<uint64>> BlockNumber();

	TFuture<TResult<FHeader>> HeaderByNumber(uint64 Id);
	TFuture<TResult<FHeader>> HeaderByNumber(EBlockTag Tag);
	TFuture<TResult<FHeader>> HeaderByHash(FHash256 Hash);
	
	TFuture<TResult<TSharedPtr<FJsonObject>>> TransactionByHash(FHash256 Hash);
	TFuture<TResult<uint64>> TransactionCount(FAddress Addr, uint64 Number);
	TFuture<TResult<uint64>> TransactionCount(FAddress Addr, EBlockTag Tag);
	TFuture<TResult<FTransactionReceipt>> TransactionReceipt(FHash256 Hash);

	TFuture<TResult<FNonUniformData>> GetGasPrice();
	TFuture<TResult<FNonUniformData>> EstimateContractCallGas(ContractCall ContractCall);
	TFuture<TResult<FNonUniformData>> EstimateDeploymentGas(FAddress from, FString Bytecode);

	TFuture<FAddress> DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId);
	
	TFuture<TResult<FBlockNonce>> NonceAt(uint64 Number);
	TFuture<TResult<FBlockNonce>> NonceAt(EBlockTag Tag);

	TFuture<FString> SendRawTransaction(FString data);
	
	TFuture<TResult<uint64>> ChainId();

	TFuture<TResult<FNonUniformData>> Call(ContractCall ContractCall, uint64 Number);
	TFuture<TResult<FNonUniformData>> Call(ContractCall ContractCall, EBlockTag Number);
	TFuture<void> NonViewCall(FEthTransaction transaction, FPrivateKey PrivateKey, int ChainID);
};


