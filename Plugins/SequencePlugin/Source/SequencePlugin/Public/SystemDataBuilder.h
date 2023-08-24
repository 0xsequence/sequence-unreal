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
	ASequence_Backend_Manager* sqncMngr;
	USyncer* masterSyncer;//keeps track of all active requests when this counts down to 0 we are done!
	UIndexer* GIndexer;
	SequenceAPI::FSequenceWallet* GWallet;
	int64 GChainId;
	FString GPublicAddress;
	FString GWalletAddress;
	FString qr_url;

	FSystemData_BE systemData;
	UObjectHandler *imageHandler;
	SequenceAPI::FSequenceWallet* sequenceAPI;
	TArray<FNFT_Master_BE> compressNFTData(TArray<FNFT_BE> nfts);

	//used for managing getting token data setup in systemdata//

	//keeps track of requests regarding itemData! ie)tokens and coins
	USyncer *getItemDataSyncer;
	void initGetItemData(FUpdatableItemDataArgs itemsToUpdate);

	//this function gets bound to the getItemDataSyncer and fires off when we are done getting image and value data for tokens
	UFUNCTION()
	void OnGetItemDataDone();

	//manager function for getting value and image data for tokens
	void initGetTokenData();

	//used for managing getting token data setup in systemdata//

	//used for managing anything that depends on the wallet address!//

	void initGetQRCode();

	//used for managing anything that depends on the wallet address//

	//master OnDone//
	/*
	* This is the master OnDone function that gets fired when system data requests are complete!
	* this function will call the needed Sequence_Backend_Manager call needed to update the front
	* with systemData!
	*/
	UFUNCTION()
		void OnDone();

	UFUNCTION()
		void OnDoneTesting();
	//master OnDone//

public:

	USystemDataBuilder();
	~USystemDataBuilder();

	void initBuildSystemData(UIndexer * indexer, SequenceAPI::FSequenceWallet * wallet,int64 chainId,FString publicAddress,ASequence_Backend_Manager * manager);

	void testGOTokenData(UIndexer* indexer, SequenceAPI::FSequenceWallet* wallet, int64 chainId, FString publicAddress);

};
