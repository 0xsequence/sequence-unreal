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
	
	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetCollectiblesWithLowestListingsFirst(const FSeqListCollectiblesArgs& Args, FOnGetCollectiblesWithLowestListingsFirst OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetAllCollectiblesWithLowestListingsFirst(const FSeqListCollectiblesArgs& Args, FOnGetAllCollectiblesWithLowestListingsFirst OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetCollectiblesWithHighestPricedOffersFirst(const FSeqListCollectiblesArgs& Args, FOnGetCollectiblesWithHighestPricedOffersFirst OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetAllCollectiblesWithHighestPricedOffersFirst(const FSeqListCollectiblesArgs& Args, FOnGetAllCollectiblesWithHighestPricedOffersFirst OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetLowestPriceOfferForCollectible(const FSeqGetCollectibleOrderArgs& Args, FOnGetLowestPriceOfferForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetHighestPriceOfferForCollectible(const FSeqGetCollectibleOrderArgs& Args, FOnGetHighestPriceOfferForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetLowestPriceListingForCollectible(const FSeqGetCollectibleOrderArgs& Args, FOnGetLowestPriceListingForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetHighestPriceListingForCollectible(const FSeqGetCollectibleOrderArgs& Args, FOnGetHighestPriceListingForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ListListingsForCollectible(const FSeqListCollectibleListingsArgs& Args, FOnListListingsForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ListAllListingsForCollectible(const FSeqListCollectibleListingsArgs& Args, FOnListAllListingsForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ListOffersForCollectible(const FSeqListCollectibleListingsArgs& Args, FOnListOffersForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ListAllOffersForCollectible(const FSeqListCollectibleListingsArgs& Args, FOnListAllOffersForCollectible OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetFloorOrder(const FSeqListCollectiblesArgs& Args, FOnGetFloorOrder OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetSwapPrice(const FGetSwapPriceArgs& Args, FOnGetSwapPrice OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetSwapPrices(const FGetSwapPricesArgs& Args, FOnGetSwapPrices OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetSwapQuote(const FGetSwapQuoteArgs& Args, FOnGetSwapQuote OnSuccess, FOnFailure OnFailure);
	
private:
	UPROPERTY()
	USequenceMarketplace* Marketplace;
};