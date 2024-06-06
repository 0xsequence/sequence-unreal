// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "FeeOption.generated.h"

UENUM(BlueprintType)
enum EFeeType
{
	Unknown UMETA(DisplayName = "unknown"),
	Erc20Token UMETA(DisplayName = "erc20token"),
	Erc1155Token UMETA(DisplayName = "erc1155token"),
};

USTRUCT()
struct FFeeToken
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	uint64 ChainID = 0;
	UPROPERTY()
	FString ContractAddress = "";
	UPROPERTY()
	uint32 Decimals = 0;
	UPROPERTY()
	FString LogoURL = "";
	UPROPERTY()
	FString Name = "";
	UPROPERTY()
	FString Symbol = "";
	UPROPERTY()
	FString TokenID = "";
	UPROPERTY()
	TEnumAsByte<EFeeType> Type = Unknown;

	FFeeToken(){}
	
	FFeeToken(uint64 ChainIDIn, const FString& ContractAddressIn, uint32 DecimalsIn,
		const FString& LogoURLIn, const FString& NameIn, const FString& SymbolIn,
		const FString& TokenIDIn, EFeeType TypeIn)
	{
		ChainID = ChainIDIn;
		ContractAddress = ContractAddressIn;
		Decimals = DecimalsIn;
		LogoURL = LogoURLIn;
		Name = NameIn;
		Symbol = SymbolIn;
		TokenID = TokenIDIn;
		Type = TypeIn;
	}
};

/*
gasLimit
to
value
"token":{"chainId":137,"contractAddress":null,"decimals":18,"logoURL":"","name":"Matic","symbol":"MATIC","tokenID":null,"type":"unknown"},"value":"1526191875889204"}
*/
USTRUCT()
struct FFeeOption
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	uint32 GasLimit = 0;
	UPROPERTY()
	FString To = "";
	UPROPERTY()
	FFeeToken Token;
	UPROPERTY()
	FString Value = "";

	FFeeOption(){}

	FFeeOption(const TSharedPtr<FJsonValue>& FeeOption)
	{
		//OR convert it to a FJsonObject and convert using UE calls instead!
		const TSharedPtr<FJsonObject> * FeeOptionObject;
		if (FeeOption->TryGetObject(FeeOptionObject))
		{
			FFeeOption FeeOptionStruct;
			if (FJsonObjectConverter::JsonObjectToUStruct<FFeeOption>(FeeOptionObject->ToSharedRef(), &FeeOptionStruct))
			{
				GasLimit = FeeOptionStruct.GasLimit;
				To = FeeOptionStruct.To;
				Token = FeeOptionStruct.Token;
				Value = FeeOptionStruct.Value;
			}
			else
			{
				UE_LOG(LogTemp,Error,TEXT("Error Parsing Fee Option"));
			}
		}
	}
	
	FFeeOption(uint32 GasLimitIn, const FString& ToIn, const FFeeToken& TokenIn, const FString& ValueIn)
	{
		GasLimit = GasLimitIn;
		To = ToIn;
		Token = TokenIn;
		Value = ValueIn;
	}
};

USTRUCT()
struct FFeeOptionReturn
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	TArray<FFeeOption> FeeOptions;
	UPROPERTY()
	bool InWallet = false;

	FFeeOptionReturn() {}
	
	FFeeOptionReturn(const TArray<FFeeOption>& FeeOptionsIn, bool InWalletIn)
	{
		FeeOptions = FeeOptionsIn;
		InWallet = InWalletIn;
	}
};

USTRUCT()
struct FIntentDataFeeOptions
{
	GENERATED_USTRUCT_BODY()
public:
	FString Identifier = "";
	FString Network = "";
	FString TransactionsString = "";//maybe look at changing this out for what gets handed in
	FString Wallet = "";
};