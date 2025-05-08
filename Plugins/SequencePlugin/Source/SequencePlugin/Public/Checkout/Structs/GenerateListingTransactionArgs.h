#pragma once
#include "CreateReq.h"
#include "Indexer/Indexer_Enums.h"
#include "Marketplace/Marketplace_Enums.h"
#include "Sequence/Wallet_Enums.h"

#include "GenerateListingTransactionArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGenerateListingTransactionArgs
{
	GENERATED_USTRUCT_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString CollectionAddress;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString Owner;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	TEnumAsByte<EContractType> ContractType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	EOrderbookKind Orderbook;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FCreateReq Listing;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	EWalletKind WalletType;

	FGenerateListingTransactionArgs()
	{
		this->CollectionAddress = "";
		this->Owner = "";
		this->ContractType = UNKOWN_CT;
		this->Orderbook = EOrderbookKind::Unknown;
		this->Listing = FCreateReq();
		this->WalletType = EWalletKind::Sequence;
	}
	
	FGenerateListingTransactionArgs(FString CollectionAddress, FString Owner, EContractType ContractType, EOrderbookKind OrderbookKind, FCreateReq Listing, EWalletKind WalletType)
	{
		this->CollectionAddress = CollectionAddress;
		this->Owner = Owner;
		this->ContractType = ContractType;
		this->Orderbook = OrderbookKind;
		this->Listing = Listing;
		this->WalletType = WalletType;
	}
	
	bool customGetter = true;
	FString GetArgs() const
	{
		FString ReturnArgs = "{";
		
		ReturnArgs += "\"collectionAddress\":\"" + CollectionAddress + "\",";
		ReturnArgs += "\"owner\":\"" + Owner + "\",";
		ReturnArgs += "\"contractType\":\"" + UEnum::GetValueAsString(ContractType) + "\",";
		ReturnArgs += "\"orderbook\":\"" + UMarketplaceEnumsExtensions::AsString(Orderbook) + "\",";
		ReturnArgs += "\"listing\":" + Listing.GetArgs() + ",";
		ReturnArgs += "\"walletType\":\"" + UMarketplaceWalletKindExtensions::AsString(WalletType) + "\"";

		ReturnArgs.Append("}");// Close the JSON object
		
		return ReturnArgs;
	}
};
