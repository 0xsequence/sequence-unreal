#pragma once
#include "AdditionalFee.h"
#include "OrderData.h"
#include "Marketplace/Marketplace_Enums.h"
#include "Sequence/Wallet_Enums.h"

#include "GenerateSellTransaction.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGenerateSellTransaction
{
	GENERATED_USTRUCT_BODY()

private:
	FString GetOrdersDataArgs() const
	{
		FString OrdersDataArgs = "";

		for (uint16 i = 0; i < OrdersData.Num(); i++)
		{
			if (i < OrdersData.Num() - 1)
			{
				OrdersDataArgs += OrdersData[i].GetArgs() + ",";
			}
			else
			{
				OrdersDataArgs += OrdersData[i].GetArgs();
			}
		}
		
		return OrdersDataArgs;
	}

	FString GetAdditionalFeesArgs() const
	{
		FString AdditionalFeesArgs = "";

		for (uint16 i = 0; i < AdditionalFees.Num(); i++)
		{
			if (i < OrdersData.Num() - 1)
			{
				AdditionalFeesArgs += AdditionalFees[i].GetArgs() + ",";
			}
			else
			{
				AdditionalFeesArgs += AdditionalFees[i].GetArgs();
			}
		}
		
		return AdditionalFeesArgs;
	}
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString CollectionAddress;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString Seller;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	TEnumAsByte<EMarketplaceKind> Marketplace;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	TArray<FOrderData> OrdersData;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	TArray<FAdditionalFee> AdditionalFees;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	EWalletKind WalletType;

	FGenerateSellTransaction()
	{
		this->CollectionAddress = "";
		this->Seller = "";
		this->Marketplace = EMarketplaceKind::SEQUENCE_MARKETPLACE_V2;
		this->OrdersData = TArray<FOrderData>();
		this->AdditionalFees = TArray<FAdditionalFee>();
		this->WalletType = EWalletKind::Unknown;
	}

	FGenerateSellTransaction(FString CollectionAddress, FString Seller, const EMarketplaceKind Marketplace, const TArray<FOrderData>& OrdersData, const TArray<FAdditionalFee>& AdditionalFees, const EWalletKind WalletType)
	{
		this->CollectionAddress = CollectionAddress;
		this->Seller = Seller;
		this->Marketplace = Marketplace;
		this->OrdersData = OrdersData;
		this->AdditionalFees = AdditionalFees;
		this->WalletType = WalletType;
	}
	
	bool customGetter = true;
	FString GetArgs() const
	{
		FString ReturnArgs = "{";

		ReturnArgs += "\"collectionAddress\":\"" + CollectionAddress + "\",";
		ReturnArgs += "\"seller\":\"" + Seller + "\",";
		ReturnArgs += "\"marketplace\":\"" + UMarketplaceEnumsExtensions::AsString(Marketplace) + "\",";
		ReturnArgs += "\"ordersData\":[" + GetOrdersDataArgs() + "],";

		if (this->AdditionalFees.IsEmpty() == false)
		{
			ReturnArgs += "\"additionalFees\":[" + GetAdditionalFeesArgs() + "],";
		}
		
		ReturnArgs += "\"walletType\":\"" + UMarketplaceWalletKindExtensions::AsString(WalletType) + "\"";

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}
};
