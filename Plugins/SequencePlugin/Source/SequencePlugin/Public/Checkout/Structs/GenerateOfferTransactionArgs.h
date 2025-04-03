#pragma once
#include "CreateReq.h"
#include "Indexer/Indexer_Enums.h"
#include "Marketplace/Marketplace_Enums.h"
#include "Sequence/Wallet_Enums.h"

#include "GenerateOfferTransactionArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGenerateOfferTransactionArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString CollectionAddress;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Maker;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EContractType> ContractType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EOrderbookKind Orderbook;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCreateReq Offer;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EWalletKind WalletType;

	FGenerateOfferTransactionArgs()
	{
		this->CollectionAddress = "";
		this->Maker = "";
		this->ContractType = EContractType::UNKOWN_CT;
		this->Orderbook = EOrderbookKind::Unknown;
		this->Offer = FCreateReq();
		this->WalletType = EWalletKind::Sequence;
	}

	FGenerateOfferTransactionArgs(FString CollectionAddress, FString Maker, const EContractType ContractType, const EOrderbookKind Orderbook, const FCreateReq& Offer, const EWalletKind WalletType)
	{
		this->CollectionAddress = CollectionAddress;
		this->Maker = Maker;
		this->ContractType = ContractType;
		this->Orderbook = Orderbook;
		this->Offer = Offer;
		this->WalletType = WalletType;
	}
	
	bool customGetter = true;
	FString GetArgs() const
	{
		FString ReturnArgs = "{";

		ReturnArgs += "\"collectionAddress\":\"" + CollectionAddress + "\",";
		ReturnArgs += "\"maker\":\"" + Maker + "\",";
		ReturnArgs += "\"contractType\":\"" + UEnum::GetValueAsString(ContractType) + "\",";
		ReturnArgs += "\"orderbook\":\"" + UMarketplaceEnumsExtensions::AsString(Orderbook) + "\",";
		ReturnArgs += "\"offer\":" + Offer.GetArgs() + ",";
		ReturnArgs += "\"WalletType\":\"" + UMarketplaceWalletKindExtensions::AsString(WalletType) + "\"";

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}
};
