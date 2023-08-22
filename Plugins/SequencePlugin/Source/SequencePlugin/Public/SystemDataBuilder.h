// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BE_Structs.h"
#include "Sequence/SequenceAPI.h"
#include "Sequence_Backend_Manager.h"
#include "SystemDataBuilder.generated.h"

/**
 * Used to build out the FSystemData struct Asynchronously!
 */
UCLASS()
class SEQUENCEPLUGIN_API USystemDataBuilder : public UObject
{
	GENERATED_BODY()
private:
	USyncer* masterSyncer;//keeps track of all active requests when this counts down to 0 we are done!
	UIndexer* GIndexer;
	SequenceAPI::FSequenceWallet* GWallet;
	int64 GChainId;
	FString GPublicAddress;

	FSystemData_BE systemData;
	UObjectHandler *imageHandler;
	SequenceAPI::FSequenceWallet* sequenceAPI;
	TArray<FNFT_Master_BE> compressNFTData(TArray<FNFT_BE> nfts);

	//keeps track of requests regarding itemData! ie)tokens and coins
	USyncer *getItemDataSyncer;
	void initGetItemData(FUpdatableItemDataArgs itemsToUpdate);

	void initGetTokenData();
public:

	USystemDataBuilder();
	~USystemDataBuilder();

	void initBuildSystemData(UIndexer * indexer, SequenceAPI::FSequenceWallet * wallet,int64 chainId,FString publicAddress);
	FSystemData_BE getSystemData();
};
