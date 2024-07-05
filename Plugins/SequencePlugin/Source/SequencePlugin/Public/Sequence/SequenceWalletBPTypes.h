// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TransakDataTypes.h"
#include "UObject/NoExportTypes.h"
#include "SequenceWalletBPTypes.generated.h"

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FTransakSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FString FiatCurrency = FDefaultTransak::FiatCurrency;

	UPROPERTY(BlueprintReadWrite)
	FString FiatAmount = FDefaultTransak::FiatAmount;

	UPROPERTY(BlueprintReadWrite)
	FString CryptoCurrency = FDefaultTransak::CryptoCurrency;

	UPROPERTY(BlueprintReadWrite)
	FString Networks = FDefaultTransak::Networks;

	UPROPERTY(BlueprintReadWrite)
	bool DisableWalletAddressForm = FDefaultTransak::DisableWalletAddressForm;

	FTransakSettings(){}
};

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