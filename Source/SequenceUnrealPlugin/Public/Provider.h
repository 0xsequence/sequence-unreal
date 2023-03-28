// Fill out your copyright notice in the Description page of Project Settings.



#pragma once

#include "CoreMinimal.h"
#include "Errors.h"
#include "UObject/Object.h"
#include "Http.h"
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
	
public:
	Provider(FString Url);
	TResult<TSharedPtr<FJsonObject>> BlockByNumber(uint16 Number);
	TResult<TSharedPtr<FJsonObject>> BlockByNumber(EBlockTag Tag);
	TResult<TSharedPtr<FJsonObject>> BlockByHash(Hash256 Hash);
	TResult<uint32> BlockNumber();
	TResult<uint32> ChainId();
};


