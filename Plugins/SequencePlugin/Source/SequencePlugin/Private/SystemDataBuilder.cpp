// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "SystemDataBuilder.h"
#include "Indexer/Indexer.h"
#include "Syncer.h"
#include "Errors.h"
#include "Sequence/SequenceAPI.h"
#include "Util/Log.h"

USystemDataBuilder::USystemDataBuilder()
{
	//Create a syncer for request management

	this->TIndexer = NewObject<UIndexer>();
	this->HIndexer = NewObject<UIndexer>();
	this->MasterSyncer = NewObject<USyncer>();
	this->MasterSyncer->SetupForTesting("master");
	this->GetItemDataSyncer = NewObject<USyncer>();
	this->GetItemDataSyncer->SetupForTesting("item");
	this->GetTxnHistorySyncer = NewObject<USyncer>();
	this->GetTxnHistorySyncer->SetupForTesting("history");
	this->QRImageHandler = NewObject<UObjectHandler>();
	this->QRImageHandler->SetupCustomFormat(true,EImageFormat::GrayscaleJPEG);//QR codes have special encodings!
	this->TokenImageHandler = NewObject<UObjectHandler>();
	this->TokenImageHandler->Setup(true);
	this->HistoryImageHandler = NewObject<UObjectHandler>();
	this->HistoryImageHandler->Setup(true);
}

USystemDataBuilder::~USystemDataBuilder()
{}

void USystemDataBuilder::DecMasterSyncer()
{
	this->MasterGuard.Lock();
	this->MasterSyncer->Decrement();
	this->MasterGuard.Unlock();
}

TArray<FNFT_Master_BE> USystemDataBuilder::CompressNftData(TArray<FNFT_BE> Nfts)
{
	TArray<FNFT_Master_BE> ret;
	TArray<FNFT_BE> lcl_nfts;
	lcl_nfts.Reserve(Nfts.Num());//prepare for append
	lcl_nfts.Append(Nfts);
	bool found = false;
	FNFT_Master_BE iNFT;

	while (lcl_nfts.Num() > 0)
	{
		found = false;
		for (FNFT_Master_BE i : ret)
		{
			if (i.NFT_Details.itemID.Compare(lcl_nfts[0].NFT_Details.itemID))
			{
				found = true;
				iNFT = i;
				break;//break out of for loop found what we needed
			}//if
		}//for

		if (found)
		{//append to existing card iNFT
			FNFT_UData_BE newUNFT;
			newUNFT.contract_address = lcl_nfts[0].NFT_Details.itemID.contractAddress;
			newUNFT.token_id = lcl_nfts[0].NFT_Details.token_id;
			iNFT.nft_data.Add(newUNFT);
		}
		else
		{//create a new card and add that to our list
			FNFT_Master_BE newMNFT;
			newMNFT.Collection_Icon = lcl_nfts[0].Collection_Icon;
			newMNFT.Collection_Icon_Url = lcl_nfts[0].Collection_Icon_URL;
			newMNFT.Collection_Long_Name = lcl_nfts[0].Collection_Long_Name;
			newMNFT.Collection_Short_Name = lcl_nfts[0].Collection_Short_Name;
			newMNFT.Description = lcl_nfts[0].Description;
			newMNFT.NFT_Details = lcl_nfts[0].NFT_Details;
			newMNFT.NFT_Icon = lcl_nfts[0].NFT_Icon;
			newMNFT.NFT_Icon_Url = lcl_nfts[0].NFT_Icon_URL;
			newMNFT.NFT_Name = lcl_nfts[0].NFT_Name;
			newMNFT.NFT_Short_Name = lcl_nfts[0].NFT_Short_Name;
			newMNFT.Properties = lcl_nfts[0].Properties;
			newMNFT.Value = lcl_nfts[0].Value;
			FNFT_UData_BE newUNFT;
			newUNFT.contract_address = lcl_nfts[0].NFT_Details.Contract_Address;
			newUNFT.token_id = lcl_nfts[0].NFT_Details.token_id;
			newMNFT.nft_data.Add(newUNFT);//add the lcl nft data!
			ret.Add(newMNFT);//now add the new master nft card to ret!
		}
		lcl_nfts.RemoveAt(0);
	}//while
	return ret;
}

/*
*	[[[ Currently non functional and will result in a soft-lock ]]]
* Gets all the value & image data injected into itemsToUpdate
*/
void USystemDataBuilder::InitGetItemData(FUpdatableItemDataArgs ItemsToUpdate)
{
	//this->getItemDataSyncer->OnDoneDelegate.BindUFunction(this,"OnGetItemDataDone");
	this->GetItemDataSyncer->Increase(3);//1 for getting images 1 for getting Coin values and 1 for getting Collectible Values
	//sequenceAPI can get all tokens and coins values in 2 calls
	//we can get all images in 1 call with Object Handler now!
	TArray<FString> urlList;
	TArray<FID_BE> idCoinList;
	TArray<FID_BE> idCollectibleList;
	//compose the URL Fetch list!
	//compose the FID_BE's into 1 big request list!
	for (FCoinUpdatable coin : ItemsToUpdate.updatingCoinData)
	{
		urlList.Add(coin.coinIconUrl);//for getting images
		idCoinList.Add(coin.coinID);//for getting updated value
	}

	for (FNFTUpdatable nft : ItemsToUpdate.updatingNftData)
	{
		urlList.Add(nft.nftCollectionIconUrl);//for getting images
		urlList.Add(nft.nftIconUrl);//for getting images
		idCollectibleList.Add(nft.nftID);//for getting updated value
	}

	this->TokenImageHandler->FOnDoneImageProcessingDelegate.BindLambda(
		[this]()
		{
			TMap<FString, UTexture2D*> images = this->TokenImageHandler->GetProcessedImages();
			this->SystemDataGuard.Lock();
			for (int32 i = 0; i < this->SystemData.user_data.coins.Num(); i++)
			{//index directly into systemData rather than jumping around
				if (images.Contains(this->SystemData.user_data.coins[i].Coin_Symbol_URL))
					this->SystemData.user_data.coins[i].Coin_Symbol = *images.Find(this->SystemData.user_data.coins[i].Coin_Symbol_URL);
			}

			for (int32 i = 0; i < this->SystemData.user_data.nfts.Num(); i++)
			{
				if (images.Contains(this->SystemData.user_data.nfts[i].NFT_Icon_Url))
					this->SystemData.user_data.nfts[i].NFT_Icon = *images.Find(this->SystemData.user_data.nfts[i].NFT_Icon_Url);

				if (images.Contains(this->SystemData.user_data.nfts[i].Collection_Icon_Url))
					this->SystemData.user_data.nfts[i].Collection_Icon = *images.Find(this->SystemData.user_data.nfts[i].Collection_Icon_Url);
			}
			this->SystemDataGuard.Unlock();
			this->ItemGuard.Lock();
			this->GetItemDataSyncer->Decrement();
			this->ItemGuard.Unlock();
		});
	this->TokenImageHandler->RequestImages(urlList);//init the requests!
	//need to compose the ID list!

	const TSuccessCallback<TArray<FItemPrice_BE>> lclCoinSuccess = [this](const TArray<FItemPrice_BE> updatedItems)
	{//because we don't get a map we have to go through everything
		TArray<FItemPrice_BE> lclUItems;
		FItemPrice_BE itemPrice;
		lclUItems.Append(updatedItems);
		while (lclUItems.Num() > 0)
		{
			itemPrice = lclUItems[0];
			this->SystemDataGuard.Lock();
			for (int32 i = 0; i < this->SystemData.user_data.coins.Num(); i++)
			{
				if (itemPrice.Token.Compare(this->SystemData.user_data.coins[i].itemID))
				{//directly inject data into system data!
					this->SystemData.user_data.coins[i].Coin_Value = itemPrice.price.value;
					break;
				}
			}
			this->SystemDataGuard.Unlock();
			lclUItems.RemoveAt(0);
		}//while
		this->ItemGuard.Lock();
		this->GetItemDataSyncer->Decrement();
		this->ItemGuard.Unlock();
	};//lambda

	const TSuccessCallback<TArray<FItemPrice_BE>> lclCollectibleSuccess = [this](const TArray<FItemPrice_BE> updatedItems)
	{//because we don't get a map we have to go through everything
		TArray<FItemPrice_BE> lclUItems;
		FItemPrice_BE itemPrice;
		lclUItems.Append(updatedItems);
		while (lclUItems.Num() > 0)
		{
			itemPrice = lclUItems[0];
			this->SystemDataGuard.Lock();
			for (int32 i = 0; i < this->SystemData.user_data.nfts.Num(); i++)
			{//index directly into systemData rather than asigning it to some inbetween party
				if (itemPrice.Token.Compare(this->SystemData.user_data.nfts[i].NFT_Details.itemID))
				{
					this->SystemData.user_data.nfts[i].Value = itemPrice.price.value;
				}
			}
			this->SystemDataGuard.Unlock();
			lclUItems.RemoveAt(0);
		}//while
		this->ItemGuard.Lock();
		this->GetItemDataSyncer->Decrement();
		this->ItemGuard.Unlock();
	};//lambda

	const FFailureCallback lclFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Error getting updated Item Prices:\n[%s]"), *Error.Message);
		this->ItemGuard.Lock();
		this->GetItemDataSyncer->Decrement();
		this->ItemGuard.Unlock();
	};

	this->WalletGuard.Lock();
	//this->GWallet->getUpdatedCoinPrices(idCoinList,lclCoinSuccess,lclFailure);
	//this->GWallet->getUpdatedCollectiblePrices(idCollectibleList, lclCollectibleSuccess, lclFailure);
	this->WalletGuard.Unlock();
}

void USystemDataBuilder::OnGetItemDataDone()
{//if we hit this function that means we are DONE getting the token data and we can decrement our master syncer!
	this->DecMasterSyncer();
}

void USystemDataBuilder::InitGetTokenData()
{
	this->GetItemDataSyncer->OnDoneDelegate.BindUFunction(this, "OnGetItemDataDone");
	const TSuccessCallback<FSeqGetTokenBalancesReturn> GenericSuccess = [&,this](const FSeqGetTokenBalancesReturn tokenBalances)
	{//once indexer responds!
		//only thing I can do is apply compression earlier for a cleaner setup
		FUpdatableItemDataArgs semiParsedTokenBalance = USequenceSupport::ExtractFromTokenBalances(tokenBalances);
		this->SystemDataGuard.Lock();
		this->SystemData.user_data.coins = semiParsedTokenBalance.semiParsedBalances.coins;
		this->SystemData.user_data.nfts = this->CompressNftData(semiParsedTokenBalance.semiParsedBalances.nfts);
		this->SystemDataGuard.Unlock();
		this->InitGetItemData(semiParsedTokenBalance);
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		//dec the request & throw error?
		this->DecMasterSyncer();
	};
	FSeqGetTokenBalancesArgs args;
	args.accountAddress = this->GWallet->GetWalletAddress();
	args.includeMetaData = true;
	this->TIndexer->GetTokenBalances(this->GWallet->GetNetworkId(), args, GenericSuccess, GenericFailure);
}

// TODO: Remove this
void USystemDataBuilder::InitGetQrCode()
{
	SEQ_LOG_EDITOR(Display, TEXT("Starting to request QR Code"));

	FString QrCodeUrl = "{0}";
	this->QRImageHandler->FOnDoneImageProcessingDelegate.BindLambda([this, QrCodeUrl]()
	{
		TMap<FString, UTexture2D*> images = this->QRImageHandler->GetProcessedImages();
		if (images.Contains(QrCodeUrl))
		{
			this->SystemData.user_data.public_qr_address = *images.Find(QrCodeUrl);
		}
		else
		{
			SEQ_LOG_EDITOR(Error, TEXT("Failed to fetch QR Code"));
		}

		this->DecMasterSyncer();
	});

	this->QRImageHandler->RequestImage(QrCodeUrl);
}

//[[[ Currently non functional and will result in a soft-lock ]]]
void USystemDataBuilder::InitGetContactData()
{
	const TSuccessCallback<TArray<FContact_BE>> GenericSuccess = [&, this](const TArray<FContact_BE> contacts)
	{//assign contact data!
		this->SystemDataGuard.Lock();
		this->SystemData.user_data.contacts = contacts;
		this->SystemDataGuard.Unlock();
		this->DecMasterSyncer();
	};

	//GO Level
	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		this->DecMasterSyncer();
	};
	this->WalletGuard.Lock();
	//this->GWallet->getFriends(this->GPublicAddress,GenericSuccess,GenericFailure);
	this->WalletGuard.Unlock();
}

/*
* We expect to receive an authable wallet, a proper chainId, and PublicAddress and a valid indexer
*/
void USystemDataBuilder::InitBuildSystemData(USequenceWallet* Wallet, ASequenceBackendManager* Manager)
{
	this->GWallet = Wallet;
	this->SqncMngr = Manager;
	this->MasterSyncer->OnDoneDelegate.BindUFunction(this, "OnDone");

	//sync operations FIRST
	//all of this we get from auth!
	this->SystemDataGuard.Lock();
	this->SystemData.user_data.public_address = Wallet->GetWalletAddress();
	this->SystemData.user_data.account_id = this->SqncMngr->GetUserDetails().AccountID;
	this->SystemData.user_data.email = this->SqncMngr->GetUserDetails().Email;
	this->SystemData.user_data.email_service = this->SqncMngr->GetUserDetails().EmailService;
	this->SystemData.user_data.username = this->SqncMngr->GetUserDetails().Username;
	FNetwork_BE default_network;
	default_network.is_default = true;
	default_network.network_name = USequenceSupport::GetNetworkName(Wallet->GetNetworkId());
	this->SystemData.user_data.networks.Add(default_network);
	this->SystemDataGuard.Unlock();
}

void USystemDataBuilder::OnDoneTesting()
{
	//here is where we will 1 print out the system data, 2 send some image data upfront to be viewed / verified
	FString result = USequenceSupport::StructToSimpleString<FSystemData_BE>(this->SystemData);
	UE_LOG(LogTemp, Display, TEXT("Parsed system data from getting token\n[%s]"), *result);
}

void USystemDataBuilder::TestGoTokenData(USequenceWallet* Wallet)
{
	this->GWallet = Wallet;
	this->MasterSyncer->OnDoneDelegate.BindUFunction(this, "OnDoneTesting");
	//ASYNC Operations next!
	this->MasterSyncer->Increase(3);//we increment outside inorder to ensure correctness in case 1 General operation finishes before the others can start
	this->InitGetTokenData();
	this->InitGetQrCode();
	this->InitGetTxnHistory();//test the history fetching
}

void USystemDataBuilder::OnDone()
{
	this->MasterGuard.Lock();
	this->MasterSyncer->OnDoneDelegate.Unbind();//for safety reasons!
	this->MasterGuard.Unlock();

	this->SystemDataGuard.Lock();
	UE_LOG(LogTemp, Display, TEXT("Before Calling update_system_data"));
	this->SqncMngr->UpdateSystemData(this->SystemData);
	UE_LOG(LogTemp, Display, TEXT("After Calling update_system_data"));
	this->SystemDataGuard.Unlock();
}

//[[[ Currently non functional and will result in a soft - lock ]] ]
void USystemDataBuilder::InitGetHistoryAuxData(FUpdatableHistoryArgs history_data)
{
	this->GetTxnHistorySyncer->Increase(3);//1 for getting images 1 for getting Coin values and 1 for getting Collectible Values
	//sequenceAPI can get all tokens and coins values in 2 calls
	//we can get all images in 1 call with Object Handler now!
	TArray<FString> urlList;
	TArray<FID_BE> idCoinList;
	TArray<FID_BE> idCollectibleList;
	//compose the URL Fetch list!
	//compose the FID_BE's into 1 big request list!
	for (FCoinUpdatable coin : history_data.updatingCoinData)
	{
		urlList.Add(coin.coinIconUrl);//for getting images
		idCoinList.Add(coin.coinID);//for getting updated value
	}

	for (FNFTUpdatable nft : history_data.updatingNftData)
	{
		urlList.Add(nft.nftCollectionIconUrl);//for getting images
		urlList.Add(nft.nftIconUrl);//for getting images
		idCollectibleList.Add(nft.nftID);//for getting updated value
	}

	this->HistoryImageHandler->FOnDoneImageProcessingDelegate.BindLambda(
		[this]()
		{
			TMap<FString, UTexture2D*> images = this->HistoryImageHandler->GetProcessedImages();

			this->SystemDataGuard.Lock();
			for (int32 i = 0; i < this->SystemData.user_data.transaction_history.Num(); i++)
			{				
				for (int j = 0; j < this->SystemData.user_data.transaction_history[i].txn_history_coins.Num(); j++)
				{
					FCoin_BE * coin = &this->SystemData.user_data.transaction_history[i].txn_history_coins[j].coin;
					if (images.Contains(coin->Coin_Symbol_URL))
					{//coin image
						coin->Coin_Symbol = *images.Find(coin->Coin_Symbol_URL);
					}
				}

				for (int j = 0; j < this->SystemData.user_data.transaction_history[i].txn_history_nfts.Num(); j++)
				{
					FNFT_BE* nft = &this->SystemData.user_data.transaction_history[i].txn_history_nfts[j].nft;
					if (images.Contains(nft->NFT_Icon_URL))
					{//assign nft image
						nft->NFT_Icon = *images.Find(nft->NFT_Icon_URL);
					}

					if (images.Contains(nft->Collection_Icon_URL))
					{//assign collection image
						nft->Collection_Icon = *images.Find(nft->Collection_Icon_URL);
					}
				}
			}//for
			this->SystemDataGuard.Unlock();
			this->HistoryGuard.Lock();
			this->GetTxnHistorySyncer->Decrement();
			this->HistoryGuard.Unlock();
		});
	this->HistoryImageHandler->RequestImages(urlList);//init the requests!
	//need to compose the ID list!

	const TSuccessCallback<TArray<FItemPrice_BE>> lclCoinSuccess = [this](const TArray<FItemPrice_BE> updatedItems)
	{//because we don't get a map we have to go through everything
		TArray<FItemPrice_BE> lclUItems;
		FItemPrice_BE itemPrice;
		lclUItems.Append(updatedItems);
		while (lclUItems.Num() > 0)
		{
			itemPrice = lclUItems[0];
			this->SystemDataGuard.Lock();
			for (int32 i = 0; i < this->SystemData.user_data.transaction_history.Num(); i++)
			{//foreach txn
				for (int32 j = 0; j < this->SystemData.user_data.transaction_history[i].txn_history_coins.Num(); j++)
				{//foreach coin in the txn
					FCoin_BE* coin = &this->SystemData.user_data.transaction_history[i].txn_history_coins[j].coin;
					if (itemPrice.Token.Compare((FID_BE)coin->itemID))
					{//if id matched
						coin->Coin_Value = itemPrice.price.value;//assign value
					}
				}
			}
			this->SystemDataGuard.Unlock();
			lclUItems.RemoveAt(0);
		}//while
		this->HistoryGuard.Lock();
		this->GetTxnHistorySyncer->Decrement();
		this->HistoryGuard.Unlock();
	};//lambda

	const TSuccessCallback<TArray<FItemPrice_BE>> lclCollectibleSuccess = [this](const TArray<FItemPrice_BE> updatedItems)
	{//because we don't get a map we have to go through everything
		TArray<FItemPrice_BE> lclUItems;
		FItemPrice_BE itemPrice;
		lclUItems.Append(updatedItems);
		while (lclUItems.Num() > 0)
		{
			itemPrice = lclUItems[0];
			this->SystemDataGuard.Lock();
			for (int32 i = 0; i < this->SystemData.user_data.transaction_history.Num(); i++)
			{//foreach txn
				for (int32 j = 0; j < this->SystemData.user_data.transaction_history[i].txn_history_nfts.Num(); j++)
				{//foreach nft in the txn
					FNFT_BE* nft = &this->SystemData.user_data.transaction_history[i].txn_history_nfts[j].nft;
					if (itemPrice.Token.Compare(nft->NFT_Details.itemID))
					{//if id matched
						nft->Value = itemPrice.price.value;//assign value
					}
				}
			}
			this->SystemDataGuard.Unlock();
			lclUItems.RemoveAt(0);
		}//while
		this->HistoryGuard.Lock();
		this->GetTxnHistorySyncer->Decrement();
		this->HistoryGuard.Unlock();
	};//lambda

	const FFailureCallback lclFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Error getting updated Item Prices:\n[%s]"), *Error.Message);
		this->HistoryGuard.Lock();
		this->GetTxnHistorySyncer->Decrement();
		this->HistoryGuard.Unlock();
	};
	this->WalletGuard.Lock();
	//this->GWallet->getUpdatedCoinPrices(idCoinList, lclCoinSuccess, lclFailure);
	//this->GWallet->getUpdatedCollectiblePrices(idCollectibleList, lclCollectibleSuccess, lclFailure);
	this->WalletGuard.Unlock();
}

void USystemDataBuilder::InitGetTxnHistory()
{
	this->GetTxnHistorySyncer->OnDoneDelegate.BindUFunction(this, "OnGetTxnHistoryDone");
	const TSuccessCallback<FSeqGetTransactionHistoryReturn> GenericSuccess = [&, this](const FSeqGetTransactionHistoryReturn history)
	{//once indexer responds!
		FUpdatableHistoryArgs semiParsedHistory = USequenceSupport::ExtractFromTransactionHistory(this->GWallet->GetWalletAddress(),history);
		this->SystemDataGuard.Lock();
		this->SystemData.user_data.transaction_history = semiParsedHistory.semiParsedHistory;//assign what we have so far!
		this->SystemDataGuard.Unlock();
		this->InitGetHistoryAuxData(semiParsedHistory);
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		//dec the request & throw error?
		this->DecMasterSyncer();
	};

	FSeqGetTransactionHistoryArgs args;
	args.includeMetaData = true;
	args.filter.accountAddress = this->GWallet->GetWalletAddress();

	this->HIndexer->GetTransactionHistory(this->GWallet->GetNetworkId(),args, GenericSuccess, GenericFailure);
}

UFUNCTION()
void USystemDataBuilder::OnGetTxnHistoryDone()
{
	this->DecMasterSyncer();
}