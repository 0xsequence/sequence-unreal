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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FiatCurrency = FDefaultTransak::FiatCurrency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FiatAmount = FDefaultTransak::FiatAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CryptoCurrency = FDefaultTransak::CryptoCurrency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Networks = FDefaultTransak::Networks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DisableWalletAddressForm = FDefaultTransak::DisableWalletAddressForm;

	FTransakSettings(){}
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSequenceResponseStatus
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Status = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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