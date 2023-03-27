// Fill out your copyright notice in the Description page of Project Settings.



#pragma once

#include "CoreMinimal.h"
#include "Errors.h"
#include "UObject/Object.h"
#include "Http.h"

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
	FString GetBlockByNumberHelper(FString Number);
	TSharedPtr<FJsonObject> Parse(FString JsonRaw);
	
public:
	Provider(FString Url);
	FString BlockByNumber(uint16 Number);
	FString BlockByNumber(EBlockTag Tag);
	TValueOrError<uint32, SequenceError> ChainId();
};
