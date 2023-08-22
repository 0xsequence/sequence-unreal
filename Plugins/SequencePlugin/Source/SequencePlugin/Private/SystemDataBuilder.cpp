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
			newMNFT.Collection_Long_Name = lcl_nfts[0].Collection_Long_Name;
			newMNFT.Collection_Short_Name = lcl_nfts[0].Collection_Short_Name;
			newMNFT.Description = lcl_nfts[0].Description;
			newMNFT.NFT_Details = lcl_nfts[0].NFT_Details;
			newMNFT.NFT_Icon = lcl_nfts[0].NFT_Icon;
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
	FUpdatableItemDataArgs* itemPtr = &itemsToUpdate;//this needs to be global if we want to do this async!
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

	//this will inject the image data //hopefully// into itemsToUpdate
	this->imageHandler->FOnDoneImageProcessingDelegate.BindLambda(
		[this,itemPtr]()
		{
			TMap<FString,UTexture2D*> images = this->imageHandler->getProcessedImages();
			
			for (FCoin_BE coin : itemPtr->semiParsedBalances.coins)
			{
				coin.Coin_Symbol = *images.Find(coin.Coin_Symbol_URL);
			}

			for (FNFT_BE nft : itemPtr->semiParsedBalances.nfts)
			{
				nft.NFT_Icon = *images.Find(nft.NFT_Icon_URL);
				nft.Collection_Icon = *images.Find(nft.Collection_Icon_URL);
			}
			//lclDec
			this->getItemDataSyncer->dec();
		});
	this->imageHandler->requestImages(urlList);//init the requests!
	//need to compose the ID list!

	const TSuccessCallback<TArray<FItemPrice_BE>> lclSuccess = [itemPtr,this](const TArray<FItemPrice_BE> updatedItems)
	{//because we don't get a map we have to go through everything
		TArray<FItemPrice_BE> lclUItems;
		FItemPrice_BE itemPrice;
		bool found = false;
		lclUItems.Append(updatedItems);
		while (lclUItems.Num() > 0)
		{
			itemPrice = lclUItems[0];
			found = false;
			for (FCoin_BE coin : itemPtr->semiParsedBalances.coins)
			{
				if (coin.itemID == itemPrice.Token)
				{
					coin.Coin_Value = itemPrice.price.value;
					found = true;//avoid uneeded searching
					break;
				}//if
			}//for
			if (!found)
			{
				for (FNFT_BE nft : itemPtr->semiParsedBalances.nfts)
				{
					if (nft.NFT_Details.itemID == itemPrice.Token)
					{
						nft.Value = itemPrice.price.value;
					}//if
				}//for
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
	//we need to setup a binding to the lclSyncer so we know when all these calls are done so we can dec the masterSyncer
	FGetTokenBalancesArgs args;
	this->GIndexer->GetTokenBalances(GChainId, args, GenericSuccess, GenericFailure);
}

/*
* We expect to receive an authable wallet, a proper chainId, and PublicAddress and a valid indexer
*/
void USystemDataBuilder::initBuildSystemData(UIndexer* indexer, SequenceAPI::FSequenceWallet* wallet, int64 chainId, FString publicAddress)
{
	this->GIndexer = indexer;
	this->GWallet = wallet;
	this->GChainId = chainId;
	this->GPublicAddress = publicAddress;

	//start of systemData construction calls!

	this->systemData.user_data.public_address = publicAddress;
}

FSystemData_BE USystemDataBuilder::getSystemData()
{
	return this->systemData;
}