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

UENUM(BlueprintType)
enum ETransactionResponseType
{
	SignMessageTrt UMETA(DisplayName = "SignMessage"),
	ValidateMessageSignatureTrt  UMETA(DisplayName = "ValidateMessageSignature"),
	GetFilteredFeeTrt UMETA(DisplayName = "GetFilteredFeeOptions"),
	GetUnFilteredFeeOptionsTrt UMETA(DisplayName = "GetUnFilteredFeeOptions"),
	SendTransactionWithFeeTrt UMETA(DisplayName = "SendTransactionWithFee"),
	SendTransactionTrt UMETA(DisplayName = "SendTransaction"),
	GetIdTokenTrt  UMETA(DisplayName = "GetIdToken"),
	ListSessionsTrt UMETA(DisplayName = "ListSessions"),
	ListAccountsTrt UMETA(DisplayName = "ListAccounts"),
	GetSessionAuthProofTrt UMETA(DisplayName = "GetSessionAuthProof"),
	GetSupportedTransakCountriesTrt UMETA(DisplayName = "GetSupportedTransakCountries"),
	PingTrt UMETA(DisplayName = "Ping"),
	VersionTrt UMETA(DisplayName = "Version"),
	RuntimeStatusTrt UMETA(DisplayName = "RuntimeStatus"),
	GetChainIdTrt UMETA(DisplayName = "GetChainId"),
	GetEtherBalanceTrt UMETA(DisplayName = "GetEtherBalances"),
	GetTokenBalancesTrt UMETA(DisplayName = "GetTokenBalances"),
	GetTokenSuppliesTrt UMETA(DisplayName = "GetTokenSupplies"),
	GetTokenSuppliesMapTrt UMETA(DisplayName = "GetTokenSuppliesMap"),
	GetBalanceUpdatesTrt UMETA(DisplayName = "GetBalanceUpdates"),
	GetTransactionHistoryTrt UMETA(DisplayName = "GetTransactionHistory"),	
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSequenceResponseStatus
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool Status = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TEnumAsByte<ETransactionResponseType> Operation = ETransactionResponseType::PingTrt;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString StatusMessage = "";

	FSequenceResponseStatus(){}

	FSequenceResponseStatus(const bool StatusIn, const ETransactionResponseType OperationIn)
	{
		Status = StatusIn;
		Operation = OperationIn;
	}

	FSequenceResponseStatus(const bool StatusIn, const FString& StatusMessageIn, const ETransactionResponseType OperationIn)
	{
		Status = StatusIn;
		StatusMessage = StatusMessageIn;
		Operation = OperationIn;
	}
};