// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SequenceWalletBPTypes.generated.h"

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSequenceResponseStatus
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	bool Status = false;
	
	UPROPERTY(BlueprintReadWrite)
	FString StatusMessage = "";

	FSequenceResponseStatus(){}

	FSequenceResponseStatus(bool StatusIn)
	{
		Status = StatusIn;
	}

	FSequenceResponseStatus(bool StatusIn, const FString& StatusMessageIn)
	{
		Status = StatusIn;
		StatusMessage = StatusMessageIn;
	}
};