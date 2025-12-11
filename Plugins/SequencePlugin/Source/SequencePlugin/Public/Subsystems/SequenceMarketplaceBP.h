#pragma once

#include "CoreMinimal.h"
#include "Marketplace/SequenceMarketplace.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Marketplace/Structs/SeqListCollectibleListingsArgs.h"
#include "SequenceMarketplaceBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceMarketplaceBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnListCurrencies, const TArray<FSeqCurrency>&, Currencies);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetCollectiblesWithLowestListingsFirst, const TArray<FSeqCollectibleOrder>&, Orders, FSeqMarketplacePage, Page);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetAllCollectiblesWithLowestListingsFirst, const TArray<FSeqCollectibleOrder>&, Orders);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetCollectiblesWithHighestPricedOffersFirst, const TArray<FSeqCollectibleOrder>&, Orders, FSeqMarketplacePage, Page);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetAllCollectiblesWithHighestPricedOffersFirst, const TArray<FSeqCollectibleOrder>&, Orders);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetLowestPriceOfferForCollectible, FSeqTokenMetaData, TokenMetadata, FSeqOrder, Order);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetHighestPriceOfferForCollectible, FSeqTokenMetaData, TokenMetadata, FSeqOrder, Order);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetLowestPriceListingForCollectible, FSeqTokenMetaData, TokenMetadata, FSeqOrder, Order);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetHighestPriceListingForCollectible, FSeqTokenMetaData, TokenMetadata, FSeqOrder, Order);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnListListingsForCollectible, const TArray<FSeqOrder>&, Orders, FSeqMarketplacePage, Page);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnListAllListingsForCollectible, const TArray<FSeqOrder>&, Orders);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnListOffersForCollectible, const TArray<FSeqOrder>&, Orders, FSeqMarketplacePage, Page);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnListAllOffersForCollectible, const TArray<FSeqOrder>&, Orders);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetFloorOrder, FSeqTokenMetaData, TokenMetadata, FSeqOrder, Order);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, FString, Error);
	
	USequenceMarketplaceBP();

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void ListCurrencies(FOnListCurrencies OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetCollectiblesWithLowestListingsFirst(const FString& ContractAddress, const FSeqCollectiblesFilter Filter, const FSeqMarketplacePage Page, FOnGetCollectiblesWithLowestListingsFirst OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetAllCollectiblesWithLowestListingsFirst(const FString& ContractAddress, const FSeqCollectiblesFilter Filter, FOnGetAllCollectiblesWithLowestListingsFirst OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetCollectiblesWithHighestPricedOffersFirst(const FString& ContractAddress, const FSeqCollectiblesFilter Filter, const FSeqMarketplacePage Page, FOnGetCollectiblesWithHighestPricedOffersFirst OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetAllCollectiblesWithHighestPricedOffersFirst(const FString& ContractAddress, const FSeqCollectiblesFilter Filter, FOnGetAllCollectiblesWithHighestPricedOffersFirst OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetLowestPriceOfferForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, FOnGetLowestPriceOfferForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetHighestPriceOfferForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, FOnGetHighestPriceOfferForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetLowestPriceListingForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, FOnGetLowestPriceListingForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetHighestPriceListingForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, FOnGetHighestPriceListingForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void ListListingsForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, const FSeqMarketplacePage Page, FOnListListingsForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void ListAllListingsForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, FOnListAllListingsForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void ListOffersForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, const FSeqMarketplacePage Page, FOnListOffersForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void ListAllOffersForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, FOnListAllOffersForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetFloorOrder(const FString& ContractAddress, const FSeqCollectiblesFilter Filter, FOnGetFloorOrder OnSuccess, FOnFailure OnFailure);

	USequenceMarketplace* GetSequenceMarketplace() const;
	
private:
	UPROPERTY()
	USequenceMarketplace* Marketplace;
};