#include "SystemDataBuilder.h"
#include "Indexer/Indexer.h"
#include "Syncer.h"
#include "Sequence/SequenceAPI.h"

USystemDataBuilder::USystemDataBuilder()
{
	//Create a syncer for request management
	this->masterSyncer = NewObject<USyncer>();
	this->getItemDataSyncer = NewObject<USyncer>();
	this->getTxnHistorySyncer = NewObject<USyncer>();
	this->QRImageHandler = NewObject<UObjectHandler>();
	this->QRImageHandler->setupCustomFormat(true,EImageFormat::GrayscaleJPEG);//QR codes have special encodings!
	this->tokenImageHandler = NewObject<UObjectHandler>();
	this->tokenImageHandler->Setup(true);
	this->HistoryImageHandler = NewObject<UObjectHandler>();
	this->HistoryImageHandler->Setup(true);
	this->sequenceAPI = new SequenceAPI::FSequenceWallet();
}

USystemDataBuilder::~USystemDataBuilder()
{
	if (this->sequenceAPI)
	{
		delete this->sequenceAPI;
	}
}

TArray<FNFT_Master_BE> USystemDataBuilder::compressNFTData(TArray<FNFT_BE> nfts)
{
	TArray<FNFT_Master_BE> ret;
	TArray<FNFT_BE> lcl_nfts;
	lcl_nfts.Reserve(nfts.Num());//prepare for append
	lcl_nfts.Append(nfts);
	bool found = false;
	FNFT_Master_BE iNFT;

	while (lcl_nfts.Num() > 0)
	{
		found = false;
		for (FNFT_Master_BE i : ret)
		{
			if (i.NFT_Details.itemID == lcl_nfts[0].NFT_Details.itemID)
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
* Gets all the value & image data injected into itemsToUpdate
*/
void USystemDataBuilder::initGetItemData(FUpdatableItemDataArgs itemsToUpdate)
{
	this->getItemDataSyncer->OnDoneDelegate.BindUFunction(this,"OnGetItemDataDone");
	this->getItemDataSyncer->incN(3);//1 for getting images 1 for getting Coin values and 1 for getting Collectible Values
	//sequenceAPI can get all tokens and coins values in 2 calls
	//we can get all images in 1 call with Object Handler now!
	TArray<FString> urlList;
	TArray<FID_BE> idCoinList;
	TArray<FID_BE> idCollectibleList;
	//compose the URL Fetch list!
	//compose the FID_BE's into 1 big request list!
	for (FCoinUpdatable coin : itemsToUpdate.updatingCoinData)
	{
		urlList.Add(coin.coinIconUrl);//for getting images
		idCoinList.Add(coin.coinID);//for getting updated value
	}

	for (FNFTUpdatable nft : itemsToUpdate.updatingNftData)
	{
		urlList.Add(nft.nftCollectionIconUrl);//for getting images
		urlList.Add(nft.nftIconUrl);//for getting images
		idCollectibleList.Add(nft.nftID);//for getting updated value
	}

	this->tokenImageHandler->FOnDoneImageProcessingDelegate.BindLambda(
		[this]()
		{
			TMap<FString, UTexture2D*> images = this->tokenImageHandler->getProcessedImages();
			for (int32 i = 0; i < this->systemData.user_data.coins.Num(); i++)
			{//index directly into systemData rather than jumping around
				if (images.Contains(this->systemData.user_data.coins[i].Coin_Symbol_URL))
					this->systemData.user_data.coins[i].Coin_Symbol = *images.Find(this->systemData.user_data.coins[i].Coin_Symbol_URL);
			}

			for (int32 i = 0; i < this->systemData.user_data.nfts.Num(); i++)
			{
				if (images.Contains(this->systemData.user_data.nfts[i].NFT_Icon_Url))
					this->systemData.user_data.nfts[i].NFT_Icon = *images.Find(this->systemData.user_data.nfts[i].NFT_Icon_Url);

				if (images.Contains(this->systemData.user_data.nfts[i].Collection_Icon_Url))
					this->systemData.user_data.nfts[i].Collection_Icon = *images.Find(this->systemData.user_data.nfts[i].Collection_Icon_Url);
			}
			this->getItemDataSyncer->dec();
		});
	this->tokenImageHandler->requestImages(urlList);//init the requests!
	//need to compose the ID list!

	const TSuccessCallback<TArray<FItemPrice_BE>> lclCoinSuccess = [this](const TArray<FItemPrice_BE> updatedItems)
	{//because we don't get a map we have to go through everything
		TArray<FItemPrice_BE> lclUItems;
		FItemPrice_BE itemPrice;
		lclUItems.Append(updatedItems);
		while (lclUItems.Num() > 0)
		{
			itemPrice = lclUItems[0];
			for (int32 i = 0; i < this->systemData.user_data.coins.Num(); i++)
			{
				if (itemPrice.Token == this->systemData.user_data.coins[i].itemID)
				{//directly inject data into system data!
					this->systemData.user_data.coins[i].Coin_Value = itemPrice.price.value;
					break;
				}
			}
			lclUItems.RemoveAt(0);
		}//while
		this->getItemDataSyncer->dec();
	};//lambda

	const TSuccessCallback<TArray<FItemPrice_BE>> lclCollectibleSuccess = [this](const TArray<FItemPrice_BE> updatedItems)
	{//because we don't get a map we have to go through everything
		TArray<FItemPrice_BE> lclUItems;
		FItemPrice_BE itemPrice;
		lclUItems.Append(updatedItems);
		while (lclUItems.Num() > 0)
		{
			itemPrice = lclUItems[0];
			for (int32 i = 0; i < this->systemData.user_data.nfts.Num(); i++)
			{//index directly into systemData rather than asigning it to some inbetween party
				if (itemPrice.Token == this->systemData.user_data.nfts[i].NFT_Details.itemID)
				{
					this->systemData.user_data.nfts[i].Value = itemPrice.price.value;
				}
			}
			lclUItems.RemoveAt(0);
		}//while
		this->getItemDataSyncer->dec();
	};//lambda

	const FFailureCallback lclFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Error getting updated Item Prices:\n[%s]"), *Error.Message);
		this->getItemDataSyncer->dec();
	};

	this->sequenceAPI->getUpdatedCoinPrices(idCoinList,lclCoinSuccess,lclFailure);
	this->sequenceAPI->getUpdatedCollectiblePrices(idCollectibleList, lclCollectibleSuccess, lclFailure);
}

void USystemDataBuilder::OnGetItemDataDone()
{//if we hit this function that means we are DONE getting the token data and we can decrement our master syncer!
	this->masterSyncer->dec();
}

void USystemDataBuilder::initGetTokenData()
{
	const TSuccessCallback<FGetTokenBalancesReturn> GenericSuccess = [&,this](const FGetTokenBalancesReturn tokenBalances)
	{//once indexer responds!
		//only thing I can do is apply compression earlier for a cleaner setup
		FUpdatableItemDataArgs semiParsedTokenBalance = UIndexerSupport::extractFromTokenBalances(tokenBalances);
		this->systemData.user_data.coins = semiParsedTokenBalance.semiParsedBalances.coins;
		this->systemData.user_data.nfts = this->compressNFTData(semiParsedTokenBalance.semiParsedBalances.nfts);
		this->initGetItemData(semiParsedTokenBalance);
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		//dec the request & throw error?
		this->masterSyncer->dec();
	};
	FGetTokenBalancesArgs args;
	args.accountAddress = this->GPublicAddress;
	args.includeMetaData = true;
	this->GIndexer->GetTokenBalances(this->GChainId, args, GenericSuccess, GenericFailure);
}

void USystemDataBuilder::initGetQRCode()
{
	//GO Level
	this->QRImageHandler->FOnDoneImageProcessingDelegate.BindLambda(
		[this]()
		{
			TMap<FString, UTexture2D*> images = this->QRImageHandler->getProcessedImages();
			if (images.Contains(this->qr_url))
			{
				if (!*images.Find(this->qr_url))
				{
					UE_LOG(LogTemp, Error, TEXT("Resolved QR to null pointer"));
				}
				this->systemData.user_data.public_qr_address = *images.Find(this->qr_url);//here we assign the QRCode we received!
			}
			else
			{//log error with getting QR code
				UE_LOG(LogTemp, Error, TEXT("Failed to fetch QR Code"));
			}
			this->masterSyncer->dec();
		});

	const TSuccessCallback<FAddress> GenericSuccess = [&, this](const FAddress address)
	{//once wallet responds with the wallet address
		this->GWalletAddress = address.ToHex();
		UE_LOG(LogTemp, Display, TEXT("Received wallet address: [%s]"), *this->GWalletAddress);
		//now we can request the QR code!
		this->qr_url = SequenceAPI::FSequenceWallet::buildQR_Request_URL(this->GWalletAddress, 512);
		this->QRImageHandler->requestImage(this->qr_url);
	};

	//GO Level
	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		this->masterSyncer->dec();
	};
	this->GWallet->GetWalletAddress(GenericSuccess,GenericFailure);
}

void USystemDataBuilder::initGetContactData()
{
	const TSuccessCallback<TArray<FContact_BE>> GenericSuccess = [&, this](const TArray<FContact_BE> contacts)
	{//assign contact data!
		this->systemData.user_data.contacts = contacts;
		this->masterSyncer->dec();//possibly one of these causing an issue?
	};

	//GO Level
	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		this->masterSyncer->dec();
	};

	this->GWallet->getFriends(this->GPublicAddress,GenericSuccess,GenericFailure);
}

/*
* We expect to receive an authable wallet, a proper chainId, and PublicAddress and a valid indexer
*/
void USystemDataBuilder::initBuildSystemData(UIndexer* indexer, SequenceAPI::FSequenceWallet* wallet, int64 chainId, FString publicAddress,ASequence_Backend_Manager * manager)
{
	this->GIndexer = indexer;
	this->GWallet = wallet;
	this->GChainId = chainId;
	this->GPublicAddress = publicAddress;
	this->sqncMngr = manager;
	this->masterSyncer->OnDoneDelegate.BindUFunction(this, "OnDone");

	//sync operations FIRST
	//all of this we get from auth!
	this->systemData.user_data.public_address = publicAddress;
	this->systemData.user_data.account_id = this->sqncMngr->getUserDetails().account_id;
	this->systemData.user_data.email = this->sqncMngr->getUserDetails().email;
	this->systemData.user_data.email_service = this->sqncMngr->getUserDetails().email_service;
	this->systemData.user_data.username = this->sqncMngr->getUserDetails().username;
	FNetwork_BE default_network;
	default_network.is_default = true;
	default_network.network_name = UIndexer::GetIndexerName(this->GChainId);
	this->systemData.user_data.networks.Add(default_network);

	

	//ASYNC Operations next!
	this->masterSyncer->incN(4);//+1 for each General Operation you have here!
	this->initGetQRCode();
	this->initGetTokenData();
	this->initGetTxnHistory();
	this->initGetContactData();
}

void USystemDataBuilder::OnDoneTesting()
{
	//here is where we will 1 print out the system data, 2 send some image data upfront to be viewed / verified
	FString result = UIndexerSupport::structToSimpleString<FSystemData_BE>(this->systemData);
	UE_LOG(LogTemp, Display, TEXT("Parsed system data from getting token\n[%s]"), *result);
}

void USystemDataBuilder::testGOTokenData(UIndexer* indexer, SequenceAPI::FSequenceWallet* wallet, int64 chainId, FString publicAddress)
{
	this->GIndexer = indexer;
	this->GWallet = wallet;
	this->GChainId = chainId;
	this->GPublicAddress = publicAddress;
	this->masterSyncer->OnDoneDelegate.BindUFunction(this, "OnDoneTesting");
	//ASYNC Operations next!
	this->masterSyncer->incN(3);//we increment outside inorder to ensure correctness in case 1 General operation finishes before the others can start
	this->initGetTokenData();
	this->initGetQRCode();
	this->initGetTxnHistory();//test the history fetching
}

void USystemDataBuilder::OnDone()
{
	this->masterSyncer->OnDoneDelegate.Unbind();//for safety reasons!
	this->sqncMngr->update_system_data(this->systemData);
}

void USystemDataBuilder::initGetHistoryAuxData(FUpdatableHistoryArgs history_data)
{
	this->getTxnHistorySyncer->OnDoneDelegate.BindUFunction(this, "OnGetTxnHistoryDone");
	this->getTxnHistorySyncer->incN(3);//1 for getting images 1 for getting Coin values and 1 for getting Collectible Values
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
			TMap<FString, UTexture2D*> images = this->HistoryImageHandler->getProcessedImages();

			for (int32 i = 0; i < this->systemData.user_data.transaction_history.Num(); i++)
			{				
				for (int j = 0; j < this->systemData.user_data.transaction_history[i].txn_history_coins.Num(); j++)
				{
					FCoin_BE * coin = &this->systemData.user_data.transaction_history[i].txn_history_coins[j].coin;
					if (images.Contains(coin->Coin_Symbol_URL))
					{//coin image
						coin->Coin_Symbol = *images.Find(coin->Coin_Symbol_URL);
					}
				}

				for (int j = 0; j < this->systemData.user_data.transaction_history[i].txn_history_nfts.Num(); j++)
				{
					FNFT_BE* nft = &this->systemData.user_data.transaction_history[i].txn_history_nfts[j].nft;
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
			this->getTxnHistorySyncer->dec();
		});
	this->HistoryImageHandler->requestImages(urlList);//init the requests!
	//need to compose the ID list!

	const TSuccessCallback<TArray<FItemPrice_BE>> lclCoinSuccess = [this](const TArray<FItemPrice_BE> updatedItems)
	{//because we don't get a map we have to go through everything
		TArray<FItemPrice_BE> lclUItems;
		FItemPrice_BE itemPrice;
		lclUItems.Append(updatedItems);
		while (lclUItems.Num() > 0)
		{
			itemPrice = lclUItems[0];

			for (int32 i = 0; i < this->systemData.user_data.transaction_history.Num(); i++)
			{//foreach txn
				for (int32 j = 0; j < this->systemData.user_data.transaction_history[i].txn_history_coins.Num(); j++)
				{//foreach coin in the txn
					FCoin_BE* coin = &this->systemData.user_data.transaction_history[i].txn_history_coins[j].coin;
					if (itemPrice.Token == coin->itemID)
					{//if id matched
						coin->Coin_Value = itemPrice.price.value;//assign value
					}
				}
			}
			lclUItems.RemoveAt(0);
		}//while
		this->getTxnHistorySyncer->dec();
	};//lambda

	const TSuccessCallback<TArray<FItemPrice_BE>> lclCollectibleSuccess = [this](const TArray<FItemPrice_BE> updatedItems)
	{//because we don't get a map we have to go through everything
		TArray<FItemPrice_BE> lclUItems;
		FItemPrice_BE itemPrice;
		lclUItems.Append(updatedItems);
		while (lclUItems.Num() > 0)
		{
			itemPrice = lclUItems[0];

			for (int32 i = 0; i < this->systemData.user_data.transaction_history.Num(); i++)
			{//foreach txn
				for (int32 j = 0; j < this->systemData.user_data.transaction_history[i].txn_history_nfts.Num(); j++)
				{//foreach nft in the txn
					FNFT_BE* nft = &this->systemData.user_data.transaction_history[i].txn_history_nfts[j].nft;
					if (itemPrice.Token == nft->NFT_Details.itemID)
					{//if id matched
						nft->Value = itemPrice.price.value;//assign value
					}
				}
			}
			lclUItems.RemoveAt(0);
		}//while
		this->getTxnHistorySyncer->dec();
	};//lambda

	const FFailureCallback lclFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Error getting updated Item Prices:\n[%s]"), *Error.Message);
		this->getTxnHistorySyncer->dec();
	};

	this->sequenceAPI->getUpdatedCoinPrices(idCoinList, lclCoinSuccess, lclFailure);
	this->sequenceAPI->getUpdatedCollectiblePrices(idCollectibleList, lclCollectibleSuccess, lclFailure);
}

void USystemDataBuilder::initGetTxnHistory()
{
	const TSuccessCallback<FGetTransactionHistoryReturn> GenericSuccess = [&, this](const FGetTransactionHistoryReturn history)
	{//once indexer responds!
		FUpdatableHistoryArgs semiParsedHistory = UIndexerSupport::extractFromTransactionHistory(this->GPublicAddress,history);
		this->systemData.user_data.transaction_history = semiParsedHistory.semiParsedHistory;//assign what we have so far!
		this->initGetHistoryAuxData(semiParsedHistory);
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		//dec the request & throw error?
		this->masterSyncer->dec();
	};

	FGetTransactionHistoryArgs args;
	args.includeMetaData = true;
	args.filter.accountAddress = this->GPublicAddress;

	this->GIndexer->GetTransactionHistory(this->GChainId,args, GenericSuccess, GenericFailure);
}

UFUNCTION()
void USystemDataBuilder::OnGetTxnHistoryDone()
{
	this->masterSyncer->dec();
}