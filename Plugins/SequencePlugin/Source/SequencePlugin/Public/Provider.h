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
	TResult<TSharedPtr<FJsonObject>> GetBlockByNumberHelper(FString Number);
	TSharedPtr<FJsonObject> Parse(FString JsonRaw);
	TResult<TSharedPtr<FJsonObject>> ExtractJsonObjectResult(FString JsonRaw);
	TResult<FString> ExtractStringResult(FString JsonRaw);
	TResult<uint64> ExtractUIntResult(FString JsonRaw);
	FString SendRPC(FString Content);
	TResult<uint64> TransactionCountHelper(FAddress Address, FString Number);
	static FJsonBuilder RPCBuilder(FString MethodName);
	TResult<FHeader> HeaderByNumberHelper(FString Number);
	TResult<FBlockNonce> NonceAtHelper(FString Number);
	TResult<FNonUniformData> CallHelper(ContractCall ContractCall, FString Number);
	
public:
	Provider(FString Url);
	TResult<TSharedPtr<FJsonObject>> BlockByNumber(uint64 Number);
	TResult<TSharedPtr<FJsonObject>> BlockByNumber(EBlockTag Tag);
	TResult<TSharedPtr<FJsonObject>> BlockByHash(FHash256 Hash);
	TResult<uint64> BlockNumber();

	TResult<FHeader> HeaderByNumber(uint64 Id);
	TResult<FHeader> HeaderByNumber(EBlockTag Tag);
	TResult<FHeader> HeaderByHash(FHash256 Hash);



	TResult<TSharedPtr<FJsonObject>> TransactionByHash(FHash256 Hash);
	TResult<uint64> TransactionCount(FAddress Addr, uint64 Number);
	TResult<uint64> TransactionCount(FAddress Addr, EBlockTag Tag);
	TResult<FTransactionReceipt> TransactionReceipt(FHash256 Hash);

	TResult<FNonUniformData> getGasPrice();
	TResult<FNonUniformData> estimateContractCallGas(ContractCall ContractCall);
	TResult<FNonUniformData> estimateDeploymentGas(FAddress from, FString Bytecode);

	FAddress DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId);
	FAddress DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TResult<FNonUniformData>& TransactionHash);
	
	TResult<FBlockNonce> NonceAt(uint64 Number);
	TResult<FBlockNonce> NonceAt(EBlockTag Tag);

	TResult<FNonUniformData> SendRawTransaction(FString data);
	
	TResult<uint64> ChainId();

	TResult<FNonUniformData> Call(ContractCall ContractCall, uint64 Number);
	TResult<FNonUniformData> Call(ContractCall ContractCall, EBlockTag Number);
	TResult<FNonUniformData> NonViewCall(FEthTransaction transaction, FPrivateKey PrivateKey, int ChainID);
};


