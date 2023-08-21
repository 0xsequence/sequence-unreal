#include "SystemDataBuilder.h"
#include "Indexer.h"
#include "Syncer.h"
#include "Sequence/SequenceAPI.h"

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

FUpdatableItemDataReturn USystemDataBuilder::getItemData(FUpdatableItemDataArgs itemsToUpdate)
{
	FUpdatableItemDataReturn ret;

	//sequenceAPI can get all tokens and coins values in 2 calls
	//the object handler could do something similar 3 calls 1 for coins, 1 for tokens, 1 for collections

	return ret;
}

/*
* We expect to receive an authable wallet, a proper chainId, and PublicAddress and a valid indexer
*/
void USystemDataBuilder::initBuildSystemData(UIndexer* indexer, SequenceAPI::FSequenceWallet* wallet, int64 chainId, FString publicAddress)
{
	//Create a syncer for request management
	USyncer* syncer = NewObject<USyncer>();

	const TSuccessCallback<FGetTokenBalancesReturn> GenericSuccess = [&](const FGetTokenBalancesReturn tokenBalances)
	{
		//need a token balances response parser!
		FUpdatableItemDataArgs semiParsedTokenBalance = UIndexerSupport::extractFromTokenBalances(tokenBalances);

		//we need requests for the following now,
		//coin value data, nft value data, coin Icon data, nft icon data, nft collection icon data
		//FUpdatableItemDataReturn ret = this->getItemData(semiParsedTokenBalance);

		const TSuccessCallback<FUpdatableItemDataReturn> lclSuccess = [&](const FUpdatableItemDataReturn updatedItems)
		{
			this->systemData.user_data.coins = updatedItems.fullyParsedBalances.coins;
			this->systemData.user_data.nfts = this->compressNFTData(updatedItems.fullyParsedBalances.nfts);
		};

		const FFailureCallback lclFailure = [](const SequenceError Error)
		{};
	};

	const FFailureCallback GenericFailure = [](const SequenceError Error)
	{ };
	
	//done creating datums for request management

	FGetTokenBalancesArgs args;
	indexer->GetTokenBalances(chainId, args, GenericSuccess, GenericFailure);

	this->systemData.user_data.public_address = publicAddress;
}

FSystemData_BE USystemDataBuilder::getSystemData()
{
	return this->systemData;
}