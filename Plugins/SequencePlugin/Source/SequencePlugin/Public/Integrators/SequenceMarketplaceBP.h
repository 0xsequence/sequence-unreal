#pragma once

#include "CoreMinimal.h"
#include "Marketplace/SequenceMarketplace.h"
#include "Marketplace/Structs/SeqGetSwapPricesArgs.h"
#include "Marketplace/Structs/SeqGetSwapQuoteArgs.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Marketplace/Structs/SeqListCollectibleListingsArgs.h"
#include "SequenceMarketplaceBP.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetCollectiblesWithLowestListingsFirst, bool, Status, const int64, ChainId, FSeqListCollectiblesReturn, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetAllCollectiblesWithLowestListingsFirst, bool, Status, const int64, ChainId, TArray<FSeqCollectibleOrder>, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetCollectiblesWithHighestPricedOffersFirst, bool, Status, const int64, ChainId, FSeqListCollectiblesReturn, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetAllCollectiblesWithHighestPricedOffersFirst, bool, Status, const int64, ChainId, TArray<FSeqCollectibleOrder>, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetLowestPriceOfferForCollectible, bool, Status, const int64, ChainId, FSeqCollectibleOrder, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetHighestPriceOfferForCollectible, bool, Status, const int64, ChainId, FSeqCollectibleOrder, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetLowestPriceListingForCollectible, bool, Status, const int64, ChainId, FSeqCollectibleOrder, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetHighestPriceListingForCollectible, bool, Status, const int64, ChainId, FSeqCollectibleOrder, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnListListingsForCollectible, bool, Status, const int64, ChainId, FSeqListCollectibleListingsReturn, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnListAllListingsForCollectible, bool, Status, const int64, ChainId, TArray<FSeqCollectibleOrder>, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnListOffersForCollectible, bool, Status, const int64, ChainId, FSeqListCollectibleOffersReturn, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnListAllOffersForCollectible, bool, Status, const int64, ChainId, TArray<FSeqCollectibleOrder>, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetFloorOrder, bool, Status, const int64, ChainId, FSeqCollectibleOrder, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetSwapPrice, bool, Status, const int64, ChainId, FSeqSwapPrice, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetSwapPrices, bool, Status, const int64, ChainId, TArray<FSeqSwapPrice>, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetSwapQuote, bool, Status, const int64, ChainId, FSeqSwapQuote, Response);

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceMarketplaceBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USequenceMarketplaceBP();

	
	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnGetCollectiblesWithLowestListingsFirst CollectiblesWithLowestListingsResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetCollectiblesWithLowestListingsFirstAsync(const int64 ChainId, const FSeqListCollectiblesArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnGetAllCollectiblesWithLowestListingsFirst AllCollectiblesWithLowestListingsResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetAllCollectiblesWithLowestListingsFirstAsync(const int64 ChainId, const FSeqListCollectiblesArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnGetCollectiblesWithHighestPricedOffersFirst CollectiblesWithHighestListingsResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetCollectiblesWithHighestPricedOffersFirstAsync(const int64 ChainId, const FSeqListCollectiblesArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnGetAllCollectiblesWithHighestPricedOffersFirst AllCollectiblesWithHighestPricedOffersResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetAllCollectiblesWithHighestPricedOffersFirstAsync(const int64 ChainId, const FSeqListCollectiblesArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnGetLowestPriceOfferForCollectible GetLowestPriceOfferForCollectibleResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetLowestPriceOfferForCollectibleAsync(const int64 ChainId, const FSeqGetCollectibleOrderArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnGetHighestPriceOfferForCollectible GetHighestPriceOfferForCollectibleResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetHighestPriceOfferForCollectibleAsync(const int64 ChainId, const FSeqGetCollectibleOrderArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnGetLowestPriceListingForCollectible GetLowestPriceListingForCollectibleResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetLowestPriceListingForCollectibleAsync(const int64 ChainId, const FSeqGetCollectibleOrderArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnGetHighestPriceListingForCollectible GetHighestPriceListingForCollectibleResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetHighestPriceListingForCollectibleAsync(const int64 ChainId, const FSeqGetCollectibleOrderArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnListListingsForCollectible FOnListListingsForCollectibleResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ListListingsForCollectibleAsync(const int64 ChainId, const FSeqListCollectibleListingsArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnListAllListingsForCollectible FOnListAllListingsForCollectibleResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ListAllListingsForCollectibleAsync(const int64 ChainId, const FSeqListCollectibleListingsArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnListOffersForCollectible FOnListOffersForCollectibleResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ListOffersForCollectibleAsync(const int64 ChainId, const FSeqListCollectibleListingsArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnListAllOffersForCollectible FOnListAllOffersForCollectibleResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ListAllOffersForCollectibleAsync(const int64 ChainId, const FSeqListCollectibleListingsArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnGetFloorOrder FOnGetFloorOrderResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetFloorOrderAsync(const int64 ChainId, const FSeqListCollectiblesArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnGetSwapPrice FOnGetSwapPriceResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetSwapPriceAsync(const int64 ChainId, const FGetSwapPriceArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnGetSwapPrices FOnGetSwapPricesResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetSwapPricesAsync(const int64 ChainId, const FGetSwapPricesArgs& Args);

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnGetSwapQuote FonGetSwapQuoteResponse;

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetSwapQuoteAsync(const int64 ChainId, const FGetSwapQuoteArgs& Args);
	
private:
	USequenceMarketplace* Marketplace;

	void CallCollectiblesWithLowestListingsFirstReceived(const bool Status, const int64 ChainId, const FSeqListCollectiblesReturn& Response);
	void CallAllCollectiblesWithLowestListingsFirstReceived(const bool Status, const int64 ChainId, const TArray<FSeqCollectibleOrder>& Response);
	void CallCollectiblesWithHighestPricedOffersFirstReceived(const bool Status, const int64 ChainId, const FSeqListCollectiblesReturn& Response);
	void CallAllCollectiblesWithHighestPricedOffersFirstReceived(const bool Status, const int64 ChainId, const TArray<FSeqCollectibleOrder>& Response);
	void CallLowestPriceOfferForCollectibleReceived(const bool Status, const int64 ChainId, const FSeqCollectibleOrder& Response);
	void CallHighestPriceOfferForCollectibleReceived(const bool Status, const int64 ChainId, const FSeqCollectibleOrder& Response);
	void CallLowestPriceListingForCollectibleReceived(const bool Status, const int64 ChainId, const FSeqCollectibleOrder& Response);
	void CallHighestPriceListingForCollectibleReceived(const bool Status, const int64 ChainId, const FSeqCollectibleOrder& Response);
	void CallListListingsForCollectibleReceived(const bool Status, const int64 ChainId, const FSeqListCollectibleListingsReturn& Response);
	void CallListAllListingsForCollectibleReceived(const bool Status, const int64 ChainId, const TArray<FSeqCollectibleOrder>& Response);
	void CallListOffersForCollectibleReceived(const bool Status, const int64 ChainId, const FSeqListCollectibleOffersReturn& Response);
	void CallListAllOffersForCollectibleReceived(const bool Status, const int64 ChainId, const TArray<FSeqCollectibleOrder>& Response);
	void CallGetFloorOrderReceived(const bool Status, const int64 ChainId, const FSeqCollectibleOrder& Response);
	void CallGetSwapPriceReceived(const bool Status, const int64 ChainId, const FSeqSwapPrice& Response);
	void CallGetSwapPricesReceived(const bool Status, const int64 ChainId, const TArray<FSeqSwapPrice>& Response);
	void CallGetSwapQuoteReceived(const bool Status, const int64 ChainId, const FSeqSwapQuote& Response);
};