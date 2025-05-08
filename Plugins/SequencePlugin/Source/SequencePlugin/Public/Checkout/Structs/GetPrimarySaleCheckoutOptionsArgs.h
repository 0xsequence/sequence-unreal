#pragma once

#include "CoreMinimal.h"
#include "CheckoutOptionsItem.h"
#include "GetPrimarySaleCheckoutOptionsArgs.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGetPrimarySaleCheckoutOptionsArgs
{
	GENERATED_USTRUCT_BODY()

private:
	FString GetItemsAsArgs() const
	{
		FString ItemsArgs = "[";

		for (int64 i = 0; i < Items.Num(); i++)
		{
			ItemsArgs += "{";
			
			ItemsArgs += "\"tokenId\":\"" + Items[i].TokenId + "\",";
			ItemsArgs += "\"quantity\":\"" + Items[i].Quantity + "\"";

			if (i < Items.Num() - 1)
			{
				ItemsArgs += "},";
			}
			else
			{
				ItemsArgs += "}";
			}
		}

		ItemsArgs.Append("]");
		
		return ItemsArgs;
	}
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString WalletAddress;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString ContractAddress;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString CollectionAddress;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	TArray<FCheckoutOptionsItem> Items;

	bool customGetter = true;
	
	FGetPrimarySaleCheckoutOptionsArgs()
	{
		this->WalletAddress = "";
		this->ContractAddress = "";
		this->CollectionAddress = "";
		this->Items = TArray<FCheckoutOptionsItem>();
	}

	FGetPrimarySaleCheckoutOptionsArgs(FString WalletAddress, FString ContractAddress, FString CollectionAddress, const TArray<FCheckoutOptionsItem>& Items)
	{
		this->WalletAddress = WalletAddress;
		this->ContractAddress = ContractAddress;
		this->CollectionAddress = CollectionAddress;
		this->Items = Items;
	}

	FGetPrimarySaleCheckoutOptionsArgs(FString WalletAddress, FString ContractAddress, FString CollectionAddress, TMap<FString, int64> Amounts)
	{
		if(Amounts.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("Must provide at least one tokenId and amount"));
			return;
		}

		TArray<FCheckoutOptionsItem> NewItems;
		NewItems.Reserve(Amounts.Num());

		int32 Index = 0;
		
		for (const TPair<FString, int64>& Item : Amounts)
		{
			FString Key = Item.Key;

			if (Amounts[Key] <= 0)
			{
				UE_LOG(LogTemp, Error, TEXT("Amount must be larger than 0, given: %lld"), Amounts[Key]);
				return;
			}

			NewItems[Index] = FCheckoutOptionsItem(Key, FString::Printf(TEXT("%lld"), Amounts[Key]));
			Index++;
		}

		this->WalletAddress = WalletAddress;
		this->ContractAddress = ContractAddress;
		this->CollectionAddress = CollectionAddress;
		this->Items = NewItems;
	}

	FGetPrimarySaleCheckoutOptionsArgs(FString WalletAddress, FString ContractAddress, FString CollectionAddress, const FString& TokenId, const int64 Amount)
	{
		if (Amount <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Amount must be larger than 0, given: %lld"), Amount);
			return;
		}

		const TArray NewItems { FCheckoutOptionsItem(TokenId, FString::Printf(TEXT("%lld"), Amount)) };
		
		this->WalletAddress = WalletAddress;
		this->ContractAddress = ContractAddress;
		this->CollectionAddress = CollectionAddress;
		this->Items = NewItems;
	}

	FString GetArgs() const
	{
		FString ReturnArgs = "{";
		ReturnArgs += "\"wallet\":\"" + WalletAddress + "\",";
		ReturnArgs += "\"contractAddress\":\"" + ContractAddress + "\",";
		ReturnArgs += "\"collectionAddress\":\"" + CollectionAddress + "\",";
		ReturnArgs += "\"items\":" + GetItemsAsArgs();
		ReturnArgs.Append("}"); // Close the JSON object

		return ReturnArgs;
	}
};
