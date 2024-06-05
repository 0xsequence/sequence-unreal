// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "FeeOption.generated.h"

UENUM(BlueprintType)
enum FeeType
{
	Unknown UMETA(DisplayName = "unknown"),
	Erc20Token UMETA(DisplayName = "erc20token"),
	Erc1155Token UMETA(DisplayName = "erc1155token"),
};

USTRUCT()
struct FeeToken
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
	TEnumAsByte<FeeType> Type = Unknown;

	FeeToken(){}
	
	FeeToken(uint64 ChainIDIn, const FString& ContractAddressIn, uint32 DecimalsIn,
		const FString& LogoURLIn, const FString& NameIn, const FString& SymbolIn,
		const FString& TokenIDIn, FeeType TypeIn)
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

USTRUCT()
struct FeeOption
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	uint32 GasLimit = 0;
	UPROPERTY()
	FString To = "";
	UPROPERTY()
	FeeToken Token;
	UPROPERTY()
	FString Value = "";

	FeeOption(){}
	
	FeeOption(uint32 GasLimitIn, const FString& ToIn, const FeeToken& TokenIn, const FString& ValueIn)
	{
		GasLimit = GasLimitIn;
		To = ToIn;
		Token = TokenIn;
		Value = ValueIn;
	}
};

USTRUCT()
struct FeeOptionReturn
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	TArray<FeeOption> FeeOptions;
	UPROPERTY()
	bool InWallet = false;

	FeeOptionReturn() {}
	
	FeeOptionReturn(const TArray<FeeOption>& FeeOptionsIn, bool InWalletIn)
	{
		FeeOptions = FeeOptionsIn;
		InWallet = InWalletIn;
	}
};