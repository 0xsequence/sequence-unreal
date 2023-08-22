#include "SystemDataBuilder.h"
#include "Indexer.h"
#include "Syncer.h"
#include "Sequence/SequenceAPI.h"

USystemDataBuilder::USystemDataBuilder()
{
	//Create a syncer for request management
	this->masterSyncer = NewObject<USyncer>();
	this->getItemDataSyncer = NewObject<USyncer>();
	//might move this to a constructor then put cleanup in destructor
	this->imageHandler = NewObject<UObjectHandler>();//create our handler!
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
	this->getItemDataSyncer->incN(2);//1 for getting images 1 for getting values
	//sequenceAPI can get all tokens and coins values in 2 calls
	//we can get all images in 1 call with Object Handler now!
	TArray<FString> urlList;
	TArray<FID_BE> idList;
	//compose the URL Fetch list!
	//compose the FID_BE's into 1 big request list!
	for (FCoinUpdatable coin : itemsToUpdate.updatingCoinData)
	{
		urlList.Add(coin.coinIconUrl);//for getting images
		idList.Add(coin.coinID);//for getting updated value
	}

	for (FNFTUpdatable nft : itemsToUpdate.updatingNftData)
	{
		urlList.Add(nft.nftCollectionIconUrl);//for getting images
		urlList.Add(nft.nftIconUrl);//for getting images
		idList.Add(nft.nftID);//for getting updated value
	}

	this->imageHandler->FOnDoneImageProcessingDelegate.BindLambda(
		[this]()
		{
			TMap<FString,UTexture2D*> images = this->imageHandler->getProcessedImages();

			for (int32 i = 0; i < this->systemData.user_data.coins.Num(); i++)
			{//index directly into systemData rather than jumping around
				this->systemData.user_data.coins[i].Coin_Symbol = *images.Find(this->systemData.user_data.coins[i].Coin_Symbol_URL);
			}
			
			for (int32 i = 0; i < this->systemData.user_data.nfts.Num(); i++)
			{
				this->systemData.user_data.nfts[i].NFT_Icon = *images.Find(this->systemData.user_data.nfts[i].NFT_Icon_Url);
				this->systemData.user_data.nfts[i].Collection_Icon = *images.Find(this->systemData.user_data.nfts[i].Collection_Icon_Url);
			}
			this->getItemDataSyncer->dec();
		});
	this->imageHandler->requestImages(urlList);//init the requests!
	//need to compose the ID list!

	const TSuccessCallback<TArray<FItemPrice_BE>> lclSuccess = [this](const TArray<FItemPrice_BE> updatedItems)
	{//because we don't get a map we have to go through everything
		TArray<FItemPrice_BE> lclUItems;
		FItemPrice_BE itemPrice;
		bool found = false;
		lclUItems.Append(updatedItems);
		while (lclUItems.Num() > 0)
		{
			itemPrice = lclUItems[0];
			found = false;
			//this->systemData.user_data.coins
			for (int32 i = 0; i < this->systemData.user_data.coins.Num(); i++)
			{
				if (itemPrice.Token == this->systemData.user_data.coins[i].itemID)
				{//directly inject data into system data!
					this->systemData.user_data.coins[i].Coin_Value = itemPrice.price.value;
					found = true;//avoid uneeded searching
					break;
				}
			}
			if (!found)
			{
				for (int32 i = 0; i < this->systemData.user_data.nfts.Num(); i++)
				{//index directly into systemData rather than asigning it to some inbetween party
					if (itemPrice.Token == this->systemData.user_data.nfts[i].NFT_Details.itemID)
					{
						this->systemData.user_data.nfts[i].Value = itemPrice.price.value;
					}
				}
			}//!found
			lclUItems.RemoveAt(0);
		}//while
		this->getItemDataSyncer->dec();
	};//lambda

	const FFailureCallback lclFailure = [this](const SequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Error getting updated Item Prices:\n[%s]"), *Error.Message);
		this->getItemDataSyncer->dec();
	};

	this->sequenceAPI->getUpdatedItemPrices(idList,lclSuccess,lclFailure);
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

	const FFailureCallback GenericFailure = [this](const SequenceError Error)
	{
		//dec the request & throw error?
		this->masterSyncer->dec();
	};

	this->masterSyncer->inc();//1 for getting the token data!
	this->masterSyncer->OnDoneDelegate.BindUFunction(this,"OnGetItemDataDone");
	FGetTokenBalancesArgs args;
	this->GIndexer->GetTokenBalances(GChainId, args, GenericSuccess, GenericFailure);
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
	this->systemData.user_data.public_address = publicAddress;

	//ASYNC Operations next!
	this->initGetTokenData();
}

void USystemDataBuilder::OnDone()
{
	this->masterSyncer->OnDoneDelegate.Unbind();//for safety reasons!
	this->sqncMngr->update_system_data(this->systemData);
}