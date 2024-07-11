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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString FiatCurrency = FDefaultTransak::FiatCurrency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString FiatAmount = FDefaultTransak::FiatAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString CryptoCurrency = FDefaultTransak::CryptoCurrency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString Networks = FDefaultTransak::Networks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool DisableWalletAddressForm = FDefaultTransak::DisableWalletAddressForm;

	FTransakSettings(){}
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSequenceResponseStatus
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool Status = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
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