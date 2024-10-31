#pragma once

#include "CoreMinimal.h"
#include "Marketplace/Marketplace.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceMarketplaceBP.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetCollectiblesWithLowestListings, bool, Status, const int64, ChainId, FSeqGetCollectiblesWithLowestListingsReturn, Response);

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceMarketplaceBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USequenceMarketplaceBP();

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnGetCollectiblesWithLowestListings CollectiblesWithLowestListingsResponse;


	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetGetCollectiblesWithLowestListingsAsync(const int64 ChainId, const FSeqGetCollectiblesWithLowestListingsArgs& Args);

private:

	UMarketplace* Marketplace;

	void CallCollectiblesWithLowestListingsReceived(const bool Status, const int64 ChainId, const FSeqGetCollectiblesWithLowestListingsReturn& Response);
};