// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Indexer/Structs/TokenBalance.h"
#include "Containers/Union.h"
#include "Util/Structs/BE_Structs.h"
#include "Indexer/IndexerSupport.h"
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

/*
 *	Reference code for how to determine which fee options to provide the user
 *         private async Task<FeeOptionsResponse> DetermineWhichFeeOptionsUserHasInWallet(IntentResponseFeeOptions feeOptions, Chain network)
        {
            try
            {
                IIndexer indexer = new ChainIndexer(network);
                int feeOptionsLength = feeOptions.feeOptions.Length;
                FeeOptionReturn[] decoratedFeeOptions = new FeeOptionReturn[feeOptionsLength];
                for (int i = 0; i < feeOptionsLength; i++)
                {
                    FeeToken token = feeOptions.feeOptions[i].token;
                    BigInteger requiredBalance = BigInteger.Parse(feeOptions.feeOptions[i].value);
                    switch (token.type)
                    {
                        case FeeTokenType.unknown:
                            EtherBalance etherBalance = await indexer.GetEtherBalance(_address);
                            decoratedFeeOptions[i] = new FeeOptionReturn(feeOptions.feeOptions[i],
                                requiredBalance <= etherBalance.balanceWei);
                            break;
                        case FeeTokenType.erc20Token:
                            GetTokenBalancesReturn tokenBalances = await indexer.GetTokenBalances(new GetTokenBalancesArgs(
                                _address, token.contractAddress));
                            if (tokenBalances.balances.Length > 0)
                            {
                                if (tokenBalances.balances[0].contractAddress != token.contractAddress)
                                {
                                    throw new Exception(
                                        $"Expected contract address from indexer response ({tokenBalances.balances[0].contractAddress}) to match contract address we queried ({token.contractAddress})");
                                }

                                decoratedFeeOptions[i] = new FeeOptionReturn(feeOptions.feeOptions[i],
                                    requiredBalance <= tokenBalances.balances[0].balance);
                            }
                            else
                            {
                                decoratedFeeOptions[i] = new FeeOptionReturn(feeOptions.feeOptions[i], false);
                            }

                            break;
                        case FeeTokenType.erc1155Token:
                            Dictionary<BigInteger, TokenBalance> sftBalances =
                                await indexer.GetTokenBalancesOrganizedInDictionary(_address, token.contractAddress,
                                    false);
                            if (sftBalances.TryGetValue(BigInteger.Parse(token.tokenID), out TokenBalance balance))
                            {
                                decoratedFeeOptions[i] = new FeeOptionReturn(feeOptions.feeOptions[i],
                                    requiredBalance <= balance.balance);
                            }
                            else
                            {
                                decoratedFeeOptions[i] = new FeeOptionReturn(feeOptions.feeOptions[i], false);
                            }

                            break;
                    }
                }

                return new FeeOptionsResponse(decoratedFeeOptions, feeOptions.feeQuote);
            }
            catch (Exception e)
            {
                throw new Exception("Unable to determine which fee option tokens the user has in their wallet: " +
                                    e.Message);
            }
        }
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
	int64 ValueNumber = 0;//Used for making easy comparisons
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
	bool CanAfford(const FFeeOption& Fee) const
	{
		bool Affordable = true;

		if (Token.Type == Fee.Token.Type && Token.Type != EFeeType::Unknown)
		{
			Affordable &= Token.ChainID == Fee.Token.ChainID;
			Affordable &= Token.ContractAddress.Compare(Fee.Token.ContractAddress) == 0;
			Affordable &= Token.Name.Compare(Fee.Token.Name) == 0;
			Affordable &= Token.Symbol.Compare(Fee.Token.Symbol) == 0;
			Affordable &= ValueNumber >= Fee.ValueNumber;
		}
		else if (Token.Type == Fee.Token.Type && Token.Type == EFeeType::Unknown)
		{//Edge case where we are looking for EtherBalance against an Unknown type of fee option
			Affordable &= ValueNumber >= Fee.ValueNumber;
		}
		
		return Affordable;
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