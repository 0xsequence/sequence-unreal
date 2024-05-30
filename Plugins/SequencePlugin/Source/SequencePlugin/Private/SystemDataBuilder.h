// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ObjectHandler.h"
#include "Util/Structs/BE_Structs.h"
#include "Sequence/SequenceAPI.h"
#include "SequenceBackendManager.h"
#include "SystemDataBuilder.generated.h"

/**
 * Used to build out the FSystemData struct Asynchronously!
 */
UCLASS()
class SEQUENCEPLUGIN_API USystemDataBuilder : public UObject
{
	GENERATED_BODY()
private:
	//Shared Variables//
	ASequenceBackendManager* SqncMngr;//this is used to let the backend know when we are done

	mutable FCriticalSection MasterGuard;//lock for masterSyncer
	USyncer* MasterSyncer;//keeps track of all active requests when this counts down to 0 we are done!
	
	// these vars are read only
	mutable FCriticalSection WalletGuard;
	USequenceWallet* GWallet;
	int64 GChainId;
	FString GPublicAddress;

	mutable FCriticalSection SystemDataGuard;//lock for system data
	FSystemData_BE SystemData;
	//Shared Variables//

	//GO Get Token Data Variables//
	UIndexer* TIndexer;
	UObjectHandler* TokenImageHandler;
	mutable FCriticalSection ItemGuard;
	USyncer* GetItemDataSyncer;
	//GO Get Token Data Variables//

	//GO Get Txn history Data Variables//
	UIndexer* HIndexer;
	mutable FCriticalSection HistoryGuard;
	USyncer* GetTxnHistorySyncer;
	UObjectHandler* HistoryImageHandler;
	//GO Get Txn history Data Variables//

	//GO Get QR Data Variables//
	FString QrURL;
	UObjectHandler* QRImageHandler;
	//GO Get QR Data Variables//

	/*
	* Used to take and Array of FNFT_BE's and convert them into a compressed form
	* FNFT_Master_BE list that the front actually uses
	*/
	TArray<FNFT_Master_BE> CompressNftData(TArray<FNFT_BE> Nfts);

	/*
	* Wrapper function for master syncer dec call,
	* this function applies locking on the syncer in a clean and easily modifiable way
	*/
	void DecMasterSyncer();


	//manager function for getting value and image data for tokens
	void InitGetTokenData();

	/*
	* Master function for controlling fetching of token data aux data such
	* as values and images and updating the needed system data values
	*/
	void InitGetItemData(FUpdatableItemDataArgs ItemsToUpdate);

	/*
	* Fires off when all token related tasks are done
	*/
	UFUNCTION()
	void OnGetItemDataDone();

	/*
	* Used for managing and setting the QR code data
	* ASYNC
	*/
	void InitGetQrCode();

	/*
	* Master controller for everything history data related
	*/
	void InitGetTxnHistory();

	/*
	* Updates the received history data with images and values
	*/
	void InitGetHistoryAuxData(FUpdatableHistoryArgs history_data);

	//Fires off when history async processing is done
	UFUNCTION()
		void OnGetTxnHistoryDone();

	//used for getting contact data! Async
	void InitGetContactData();

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

	void InitBuildSystemData(USequenceWallet *Wallet, int64 ChainId,FString PublicAddress, ASequenceBackendManager* Manager);

	void TestGoTokenData(USequenceWallet* Wallet, int64 ChainId, FString PublicAddress);

};
