// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
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
	ASequenceBackendManager* sqncMngr;//this is used to let the backend know when we are done

	mutable FCriticalSection masterGuard;//lock for masterSyncer
	USyncer* masterSyncer;//keeps track of all active requests when this counts down to 0 we are done!
	
	// these vars are read only
	mutable FCriticalSection walletGuard;
	SequenceAPI::FSequenceWallet* GWallet;
	int64 GChainId;
	FString GPublicAddress;

	mutable FCriticalSection systemDataGuard;//lock for system data
	FSystemData_BE systemData;
	//Shared Variables//

	//GO Get Token Data Variables//
	UIndexer* tIndexer;
	UObjectHandler* tokenImageHandler;
	mutable FCriticalSection itemGuard;
	USyncer* getItemDataSyncer;
	//GO Get Token Data Variables//

	//GO Get Txn history Data Variables//
	UIndexer* hIndexer;
	mutable FCriticalSection historyGuard;
	USyncer* getTxnHistorySyncer;
	UObjectHandler* HistoryImageHandler;
	//GO Get Txn history Data Variables//

	//GO Get QR Data Variables//
	FString qr_url;
	UObjectHandler* QRImageHandler;
	//GO Get QR Data Variables//

	/*
	* Used to take and Array of FNFT_BE's and convert them into a compressed form
	* FNFT_Master_BE list that the front actually uses
	*/
	TArray<FNFT_Master_BE> compressNFTData(TArray<FNFT_BE> nfts);

	/*
	* Wrapper function for master syncer dec call,
	* this function applies locking on the syncer in a clean and easily modifiable way
	*/
	void decMasterSyncer();


	//manager function for getting value and image data for tokens
	void initGetTokenData();

	/*
	* Master function for controlling fetching of token data aux data such
	* as values and images and updating the needed system data values
	*/
	void initGetItemData(FUpdatableItemDataArgs itemsToUpdate);

	/*
	* Fires off when all token related tasks are done
	*/
	UFUNCTION()
	void OnGetItemDataDone();

	/*
	* Used for managing and setting the QR code data
	* ASYNC
	*/
	void initGetQRCode();

	/*
	* Master controller for everything history data related
	*/
	void initGetTxnHistory();

	/*
	* Updates the received history data with images and values
	*/
	void initGetHistoryAuxData(FUpdatableHistoryArgs history_data);

	//Fires off when history async processing is done
	UFUNCTION()
		void OnGetTxnHistoryDone();

	//used for getting contact data! Async
	void initGetContactData();

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

	void initBuildSystemData(SequenceAPI::FSequenceWallet * wallet,int64 chainId,FString publicAddress, ASequenceBackendManager* manager);

	void testGOTokenData(SequenceAPI::FSequenceWallet* wallet, int64 chainId, FString publicAddress);

};
