#pragma once

#include "CoreMinimal.h"
#include "Marketplace/SequenceMarketplace.h"
#include "Marketplace/Structs/SeqGetSwapPricesArgs.h"
#include "Marketplace/Structs/SeqGetSwapQuoteArgs.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Marketplace/Structs/SeqListCollectibleListingsArgs.h"
#include "SequenceMarketplaceBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceMarketplaceBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnListCurrencies, FSeqListCurrenciesReturn, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetCollectiblesWithLowestListingsFirst, FSeqListCollectiblesReturn, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetAllCollectiblesWithLowestListingsFirst, TArray<FSeqCollectibleOrder>, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetCollectiblesWithHighestPricedOffersFirst, FSeqListCollectiblesReturn, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetAllCollectiblesWithHighestPricedOffersFirst, TArray<FSeqCollectibleOrder>, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetLowestPriceOfferForCollectible, FSeqCollectibleOrder, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetHighestPriceOfferForCollectible, FSeqCollectibleOrder, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetLowestPriceListingForCollectible, FSeqCollectibleOrder, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetHighestPriceListingForCollectible, FSeqCollectibleOrder, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnListListingsForCollectible, FSeqListCollectibleListingsReturn, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnListAllListingsForCollectible, TArray<FSeqCollectibleOrder>, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnListOffersForCollectible, FSeqListCollectibleOffersReturn, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnListAllOffersForCollectible, TArray<FSeqCollectibleOrder>, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetFloorOrder, FSeqCollectibleOrder, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSwapPrice, FSeqSwapPrice, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSwapPrices, TArray<FSeqSwapPrice>, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSwapQuote, FSeqSwapQuote, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, FString, Error);

public:
	USequenceMarketplaceBP();

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void ListCurrencies(FOnListCurrencies OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetCollectiblesWithLowestListingsFirst(const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, FOnFailure OnFailure, FOnGetCollectiblesWithLowestListingsFirst OnSuccess);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetAllCollectiblesWithLowestListingsFirst(const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, FOnGetAllCollectiblesWithLowestListingsFirst OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetCollectiblesWithHighestPricedOffersFirst(const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, FOnGetCollectiblesWithHighestPricedOffersFirst OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetAllCollectiblesWithHighestPricedOffersFirst(const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, FOnGetAllCollectiblesWithHighestPricedOffersFirst OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetLowestPriceOfferForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, FOnGetLowestPriceOfferForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetHighestPriceOfferForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, FOnGetHighestPriceOfferForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetLowestPriceListingForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, FOnGetLowestPriceListingForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetHighestPriceListingForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, FOnGetHighestPriceListingForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void ListListingsForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, FOnListListingsForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void ListAllListingsForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, FOnListAllListingsForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void ListOffersForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, FOnListOffersForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void ListAllOffersForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, FOnListAllOffersForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetFloorOrder(const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, FOnGetFloorOrder OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetSwapPrice(const FString& SellCurrency, const FString& BuyCurrency, const FString& BuyAmount, const int SlippagePercentage, FOnGetSwapPrice OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetSwapPrices(const FString& UserWallet, const FString& BuyCurrency, const FString& BuyAmount, const int SlippagePercentage, FOnGetSwapPrices OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Marketplace")
	void GetSwapQuote(const FString& UserWallet, const FString& BuyCurrency, const FString& SellCurrency, const FString& BuyAmount, const bool IncludeApprove, const int SlippagePercentage, FOnGetSwapQuote OnSuccess, FOnFailure OnFailure);
	
private:
	UPROPERTY()
	USequenceMarketplace* Marketplace;
};