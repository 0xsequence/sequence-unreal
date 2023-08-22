// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BE_Structs.h"
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
	FSystemData_BE systemData;

	TArray<FNFT_Master_BE> compressNFTData(TArray<FNFT_BE> nfts);

	FUpdatableItemDataReturn getItemData(FUpdatableItemDataArgs itemsToUpdate);
public:
	void initBuildSystemData(UIndexer * indexer, SequenceAPI::FSequenceWallet * wallet,int64 chainId,FString publicAddress);
	FSystemData_BE getSystemData();
};
