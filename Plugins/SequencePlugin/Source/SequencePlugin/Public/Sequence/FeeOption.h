// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Indexer/Structs/TokenBalance.h"
#include "Containers/Union.h"
#include "Util/Structs/BE_Structs.h"
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
	int32 Decimals = 0;
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
	
	FFeeToken(uint64 ChainIDIn, const FString& ContractAddressIn, int32 DecimalsIn,
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
	int64 ValueNumber = 0;//Used for making easy comparisons
	bool bCanAfford = false;
	FFeeOption(){}

	FFeeOption(const FEtherBalance& EtherBalance)
	{
		ValueNumber = EtherBalance.balanceWei;
		Token.Type = EFeeType::Unknown;
	}
	
	FFeeOption(const FTokenBalance& BalanceOption)
	{
		Value = "";
		Value.AppendInt(BalanceOption.balance);
		Token.Decimals = BalanceOption.contractInfo.decimals;
		Token.Name = BalanceOption.contractInfo.name;
		Token.Symbol = BalanceOption.contractInfo.symbol;
		Token.ContractAddress = BalanceOption.contractAddress;
		Token.ChainID = BalanceOption.chainId;
		Token.LogoURL = BalanceOption.contractInfo.logoURI;
		ValueNumber = BalanceOption.balance;
		
		switch(BalanceOption.contractType)
		{
		case EContractType::ERC20:
			Token.Type = EFeeType::Erc20Token;
			break;
		case EContractType::ERC1155:
			Token.Type = EFeeType::Erc1155Token;
			break;		
		default:
			Token.Type = EFeeType::Unknown;
			break;
		}
	}
	
	FFeeOption(const TSharedPtr<FJsonValue>& FeeOption)
	{
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
				ValueNumber = FCString::Atoi64(*Value);
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

	/*
	 * Compare our values against the fee provided
	 * if our values are valid & the Fee's values are valid &
	 * Our numerical values are greater than or equal to the fee
	 * we can afford said fee
	 */
	bool CanAfford(const FFeeOption& Fee)
	{
		bool Affordable = true;

		if (Token.Type == Fee.Token.Type && Token.Type != EFeeType::Unknown)
		{
			Affordable &= Token.ChainID == Fee.Token.ChainID;
			Affordable &= Token.ContractAddress.Equals(Fee.Token.ContractAddress,ESearchCase::IgnoreCase);
			Affordable &= Token.Name.Equals(Fee.Token.Name,ESearchCase::IgnoreCase);
			Affordable &= Token.Symbol.Equals(Fee.Token.Symbol,ESearchCase::IgnoreCase);
			Affordable &= ValueNumber >= Fee.ValueNumber;
		}
		else if (Token.Type == Fee.Token.Type && Token.Type == EFeeType::Unknown)
		{//Edge case where we are looking for EtherBalance against an Unknown type of fee option
			Affordable &= Token.Name.Equals(Fee.Token.Name,ESearchCase::IgnoreCase);
			Affordable &= Token.Symbol.Equals(Fee.Token.Symbol,ESearchCase::IgnoreCase);
			Affordable &= ValueNumber >= Fee.ValueNumber;
		}
		else
		{
			Affordable = false;
		}
		bCanAfford = Affordable;
		return Affordable;
	}

	TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction> CreateTransaction()
	{
		TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction> Transaction;
		switch(Token.Type)
		{
		case EFeeType::Unknown:
			Transaction.SetSubtype<FRawTransaction>(FRawTransaction(To,"",Value));
			break;
		case EFeeType::Erc20Token:
			Transaction.SetSubtype<FERC20Transaction>(FERC20Transaction(To,Value,Token.ContractAddress));
			break;
		case EFeeType::Erc1155Token:
			Transaction.SetSubtype<FERC1155Transaction>(FERC1155Transaction(To,"",Token.ContractAddress,{ FERC1155TxnValue(Value,Token.TokenID) }));
			break;
		default:
			FRawTransaction FailedOption;
			FailedOption.to = To;
			FailedOption.value = Value;
			FailedOption.data = "";
			Transaction.SetSubtype<FRawTransaction>(FailedOption);
			break;
		}
		return Transaction;
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
	TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> Transactions;
	FString Wallet = "";
};