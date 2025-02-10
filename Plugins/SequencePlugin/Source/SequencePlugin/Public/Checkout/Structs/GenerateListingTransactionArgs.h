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
	FString CollectionAddress;
	FString Owner;
	EContractType ContractType;
	EOrderbookKind Orderbook;
	FCreateReq Listing;
	EWalletKind WalletType;

	FGenerateListingTransactionArgs()
	{
		this->CollectionAddress = "";
		this->Owner = "";
		this->ContractType = EContractType::UNKOWN_CT;
		this->Orderbook = EOrderbookKind::Unknown;
		this->Listing = FCreateReq();
		this->WalletType = EWalletKind::Sequence;
	}
	
	FGenerateListingTransactionArgs(FString CollectionAddress, FString Owner, EContractType ContractType, EOrderbookKind OrderbookKind, FCreateReq Listing, EWalletKind WalletType)
	{
		this->CollectionAddress = CollectionAddress;
		this->Owner = Owner;
		this->ContractType = ContractType;
		this->Orderbook = Orderbook;
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
		ReturnArgs += "\"orderbook\":\"" + UEnum::GetValueAsString(Orderbook) + "\",";
		ReturnArgs += "\"listing\":\"" + Listing.GetArgs() + "\",";
		ReturnArgs += "\"WalletType\":\"" + UEnum::GetValueAsString(WalletType) + "\"";

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}
};
