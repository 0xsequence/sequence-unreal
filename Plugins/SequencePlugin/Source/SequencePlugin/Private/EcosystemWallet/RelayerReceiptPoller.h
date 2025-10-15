// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Relayer/SequenceRelayer.h"
#include "UObject/Object.h"
#include "RelayerReceiptPoller.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API URelayerReceiptPoller : public UObject
{
	GENERATED_BODY()

public:
	void StartPolling(USequenceRelayer& InRelayer, const FString& Hash, const TFunction<void(FString)>& InOnSuccess, const TFunction<void(FString)>& InOnFailure);

private:
	void PollRecursive(const FString& Hash);

	UPROPERTY()
	USequenceRelayer* Relayer;

	TFunction<void(FString)> OnSuccess;
	TFunction<void(FString)> OnFailure;
	
	FString CurrentStatus = "Pending";
};
