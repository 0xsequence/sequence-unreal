// Fill out your copyright notice in the Description page of Project Settings.



#pragma once

#include "CoreMinimal.h"
#include "BinaryData.h"
#include "Errors.h"
#include "Header.h"
#include "Http.h"
#include "JsonBuilder.h"
#include "Types.h"

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
	TResult<uint32> ExtractUInt32Result(FString JsonRaw);
	FString SendRPC(FString Content);
	TResult<uint32> TransactionCountHelper(FAddress Address, FString Number);
	static FJsonBuilder RPCBuilder(FString MethodName);
	TResult<FHeader> HeaderByNumberHelper(FString Number);
	TResult<FBlockNonce> NonceAtHelper(FString Number);
	
public:
	Provider(FString Url);
	TResult<TSharedPtr<FJsonObject>> BlockByNumber(uint16 Number);
	TResult<TSharedPtr<FJsonObject>> BlockByNumber(EBlockTag Tag);
	TResult<TSharedPtr<FJsonObject>> BlockByHash(FHash256 Hash);
	TResult<uint32> BlockNumber();

	TResult<FHeader> HeaderByNumber(uint16 Id);
	TResult<FHeader> HeaderByNumber(EBlockTag Tag);
	TResult<FHeader> HeaderByHash(FHash256 Hash);

	TResult<TSharedPtr<FJsonObject>> TransactionByHash(FHash256 Hash);
	TResult<uint32> TransactionCount(FAddress Addr, uint16 Number);
	TResult<uint32> TransactionCount(FAddress Addr, EBlockTag Tag);
	TResult<TSharedPtr<FJsonObject>> TransactionReceipt(FHash256 Hash);
	
	TResult<FBlockNonce> NonceAt(uint16 Number);
	TResult<FBlockNonce> NonceAt(EBlockTag Tag);

	void SendRawTransaction(FString data);
	
	TResult<uint32> ChainId();
};


