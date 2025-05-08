#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Indexer/Structs/SeqTokenBalance.h"
#include "Containers/Union.h"
#include "Indexer/Structs/SeqEtherBalance.h"
#include "Util/Log.h"
#include "Util/Structs/BE_Structs.h"
#include "FeeOption.generated.h"

UENUM(BlueprintType)
enum EFeeType
{
	Unknown UMETA(DisplayName = "unknown"),
	Erc20Token UMETA(DisplayName = "erc20token"),
	Erc1155Token UMETA(DisplayName = "erc1155token"),
};

USTRUCT(BlueprintType)
struct FFeeToken
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	int64 ChainID = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString ContractAddress = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	int32 Decimals = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString LogoURL = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Name = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Symbol = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString TokenID = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	TEnumAsByte<EFeeType> Type = Unknown;

	FFeeToken(){}
	
	FFeeToken(int64 ChainIDIn, const FString& ContractAddressIn, int32 DecimalsIn,
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

USTRUCT(BlueprintType)
struct FFeeOption
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	int64 GasLimit = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString To = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FFeeToken Token;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Value = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	int64 ValueNumber = 0;//Used for making easy comparisons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	bool bCanAfford = false;
	FFeeOption(){}

	FFeeOption(const FSeqEtherBalance& EtherBalance)
	{
		ValueNumber = EtherBalance.balanceWei;
		Token.Type = EFeeType::Unknown;
	}
	
	FFeeOption(const FSeqTokenBalance& BalanceOption)
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
				SEQ_LOG(Error,TEXT("Error Parsing Fee Option"));
			}
		}
	}
	
	FFeeOption(int64 GasLimitIn, const FString& ToIn, const FFeeToken& TokenIn, const FString& ValueIn)
	{
		GasLimit = GasLimitIn;
		To = ToIn;
		Token = TokenIn;
		Value = ValueIn;
	}

	TransactionUnion CreateTransaction()
	{
		TransactionUnion Transaction;
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
	
	bool Equals(const FFeeOption& Fee) const
	{
		bool IsMatch = true;
		
		if (Token.Type == Fee.Token.Type && Token.Type == EFeeType::Erc20Token)
		{
			IsMatch &= Token.ChainID == Fee.Token.ChainID;
			IsMatch &= Token.ContractAddress.Equals(Fee.Token.ContractAddress,ESearchCase::IgnoreCase);
		}
		else if (Token.Type == Fee.Token.Type && Token.Type == EFeeType::Erc1155Token)
		{
			IsMatch &= Token.ChainID == Fee.Token.ChainID;
			IsMatch &= Token.ContractAddress.Equals(Fee.Token.ContractAddress,ESearchCase::IgnoreCase);
			IsMatch &= Token.TokenID.Equals(Fee.Token.TokenID, ESearchCase::IgnoreCase);
		}
		else if (Token.Type == Fee.Token.Type && Token.Type == EFeeType::Unknown)
		{
			IsMatch = true;
		}
		else
		{
			IsMatch = false;
		}
		return IsMatch;
	}
	
	/*
	* Compare our values against the fee provided
	* if our values are valid & the Fee's values are valid &
	* Our numerical values are greater than or equal to the fee
	* we can afford said fee
	*/
	bool CanAfford(const FFeeOption& Fee)
	{
		bCanAfford = Equals(Fee) && ValueNumber >= Fee.ValueNumber;
		return bCanAfford;
	}
};