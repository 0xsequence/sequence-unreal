// Fill out your copyright notice in the Description page of Project Settings.

#include "IndexerSupport.h"
#include "Version.h"
#include "GetTransactionHistoryReturn.h"
#include "Struct_Data.h"
#include "BE_Structs.h"
#include "Indexer.h"

/*
* This will convert a jsonObject into a TMap<FString,FString> thereby making a dynamic
* object usable in the UI!
*/
TMap<FString, FString> UIndexerSupport::jsonObjectParser(TSharedPtr<FJsonObject> jsonData)
{
	TArray<TPair<FString, TSharedPtr<FJsonValue>>> jsonList = jsonData.Get()->Values.Array();
	TMap<FString, FString> ret;
	ret.Reserve(jsonList.Num());//put some slack in to speed this up!

	for (TPair<FString,TSharedPtr<FJsonValue>> kvp : jsonList)
	{
		TPair<FString, FString> tPair(kvp.Key, stringCleanup(jsonToString(kvp.Value)));
		ret.Add(tPair);
	}
	return ret;
}

FString UIndexerSupport::simplifyString(FString string)
{
	FString* ret = &string;

	FString srch_n = TEXT("\n");//we want no returns the UI will take of this for us!
	FString srch_r = TEXT("\r");
	FString srch_t = TEXT("\t");
	FString srch_c = TEXT("\"");//this will be apart of json formatting get rid of it!
	FString replace = TEXT("");

	const TCHAR* srch_ptr_n = *srch_n;
	const TCHAR* srch_ptr_r = *srch_r;
	const TCHAR* srch_ptr_t = *srch_t;
	const TCHAR* srch_ptr_c = *srch_c;
	const TCHAR* rep_ptr = *replace;

	(*ret).RemoveSpacesInline();//remove spaces
	(*ret).ReplaceInline(srch_ptr_n, rep_ptr, ESearchCase::IgnoreCase);//remove \n
	(*ret).ReplaceInline(srch_ptr_r, rep_ptr, ESearchCase::IgnoreCase);//remove \r
	(*ret).ReplaceInline(srch_ptr_t, rep_ptr, ESearchCase::IgnoreCase);//remove \t
	(*ret).ReplaceInline(srch_ptr_c, rep_ptr, ESearchCase::IgnoreCase);//remove \"

	return (*ret);
}

FString UIndexerSupport::simplifyStringParsable(FString string)
{
	FString* ret = &string;

	FString srch_n = TEXT("\n");//we want no returns the UI will take of this for us!
	FString srch_r = TEXT("\r");
	FString srch_t = TEXT("\t");
	FString replace = TEXT("");

	const TCHAR* srch_ptr_n = *srch_n;
	const TCHAR* srch_ptr_r = *srch_r;
	const TCHAR* srch_ptr_t = *srch_t;
	const TCHAR* rep_ptr = *replace;

	(*ret).RemoveSpacesInline();//remove spaces
	(*ret).ReplaceInline(srch_ptr_n, rep_ptr, ESearchCase::IgnoreCase);//remove \n
	(*ret).ReplaceInline(srch_ptr_r, rep_ptr, ESearchCase::IgnoreCase);//remove \r
	(*ret).ReplaceInline(srch_ptr_t, rep_ptr, ESearchCase::IgnoreCase);//remove \t

	return (*ret);
}

FString UIndexerSupport::stringListToSimpleString(TArray<FString> stringData)
{
	FString ret = "[";

	for (FString string : stringData)
	{
		ret += string;
		ret += ",";
	}

	if (stringData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);//remove the last comma as it'll be wrong!
	}
	ret += "]";
	return ret;
}

FString UIndexerSupport::stringListToParsableString(TArray<FString> stringData)
{
	FString ret = "[";
	for (FString string : stringData)
	{
		ret += "\""+string+"\"";
		ret += ",";
	}

	if (stringData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);//remove the last comma as it'll be wrong!
	}
	ret += "]";
	return ret;
}

FString UIndexerSupport::int64ListToSimpleString(TArray<int64> intData)
{
	FString ret = "[";

	for (int64 iData : intData)
	{
		FString iDataString = FString::Printf(TEXT("%lld"), iData);
		ret += iDataString + ",";
	}

	if (intData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);//remove the last comma as it'll be wrong!
	}

	ret += "]";
	return ret;
}

FString UIndexerSupport::jsonObjListToString(TArray<TSharedPtr<FJsonObject>> jsonData)
{
	FString ret = "[";
	for (TSharedPtr<FJsonObject> jObj : jsonData)
	{
		ret.Append(UIndexerSupport::jsonToString(jObj));
		ret.Append(",");
	}

	if (jsonData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);
	}
	ret.Append("]");
	return ret;
}

FString UIndexerSupport::jsonObjListToSimpleString(TArray<TSharedPtr<FJsonObject>> jsonData)
{
	FString ret = "[";
	for (TSharedPtr<FJsonObject> jObj : jsonData)
	{
		ret.Append(UIndexerSupport::jsonToSimpleString(jObj));
		ret.Append(",");
	}

	if (jsonData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);
	}
	ret.Append("]");
	return ret;
}

FString UIndexerSupport::jsonObjListToParsableString(TArray<TSharedPtr<FJsonObject>> jsonData)
{
	FString ret = "[";
	for (TSharedPtr<FJsonObject> jObj : jsonData)
	{
		ret.Append(UIndexerSupport::jsonToParsableString(jObj));
		ret.Append(",");
	}

	if (jsonData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);
	}
	ret.Append("]");
	return ret;
}

FString UIndexerSupport::jsonToString(TSharedPtr<FJsonObject> jsonData)
{
	FString ret;
	TSharedRef< TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ret);
	FJsonSerializer::Serialize(jsonData.ToSharedRef(), Writer);
	return ret;
}

FString UIndexerSupport::jsonToSimpleString(TSharedPtr<FJsonValue> jsonData)
{
	return simplifyString(jsonToString(jsonData));
}

FString UIndexerSupport::jsonToSimpleString(TSharedPtr<FJsonObject> jsonData)
{
	return simplifyString(jsonToString(jsonData));
}

void UIndexerSupport::ExtractFromTransactionHistory(FString MyAddress, FGetTransactionHistoryReturn TransactionHistory,
	TSuccessCallback<TArray<FTransactionHistoryItem_BE>> OnSuccess, FFailureCallback OnFailure)
{
	TArray<FTransactionHistoryItem_BE> Array;

	for(FTransaction Transaction : TransactionHistory.transactions)
	{
		FTransactionHistoryItem_BE Item;
		Item.network_name = *UIndexer::GetIndexerNames().Find(Transaction.chainId);
		Item.network_icon = nullptr;
		bool TxnTypeSet = false;
		
		for(FTxnTransfer Transfer : Transaction.transfers)
		{
			FTokenMetaData TokenMetaData = Transfer.tokenMetaData.Array()[0].Value;
			Item.other_public_address = Transfer.from == MyAddress ? Transfer.to : Transfer.from;
			Item.other_icon = nullptr;

			if(!TxnTypeSet)
			{
				Item.transaction_type = Transfer.transferType == SEND ? TXN_Send : TXN_Receive;
				TxnTypeSet = true;
			}
			else if(Item.transaction_type != TXN_Swap)
			{
				if(Item.transaction_type == TXN_Receive && Transfer.transferType == SEND)
				{
					Item.transaction_type = TXN_Swap;
				}
				else if(Item.transaction_type == TXN_Send && Transfer.transferType == RECEIVE)
				{
					Item.transaction_type = TXN_Swap;
				}
			}
			
			Item.transaction_date = TimestampToMonthDayYear_Be(Transaction.timestamp);

			if(Transfer.contractType == ERC20 || Transfer.contractType == ERC1155 || Transfer.contractType == ERC1155_BRIDGE || Transfer.contractType == ERC20_BRIDGE)
			{
				FCoinTxn_BE CoinTxn;
				
				//coin
				CoinTxn.amount = Transfer.amounts[0] * FMath::Pow(10.0, Transfer.contractInfo.decimals);
				CoinTxn.coin.Coin_Short_Name = Transfer.contractInfo.logoURI;
				CoinTxn.coin.Coin_Long_Name = Transfer.contractInfo.name;
				CoinTxn.coin.Coin_Amount = CoinTxn.amount;
				CoinTxn.coin.Coin_Standard = Transfer.contractType;
				CoinTxn.coin.Coin_Value = 1.0; // unknown, set to 1.0 for now

				Item.txn_history_coins.Add(CoinTxn);
			}
			else
			{
				// Assume its an NFT?
				FNFTTxn_BE NftTxn;

				NftTxn.amount = Transfer.amounts[0] * FMath::Pow(10.0, Transfer.contractInfo.decimals);
				NftTxn.nft.Amount = NftTxn.amount;
				NftTxn.nft.Description = Transfer.contractInfo.extensions.description;
				NftTxn.nft.Properties; // unknown, probably in token meta data
				NftTxn.nft.Value = 1.0; // unknown, set to 1.0 for now
				NftTxn.nft.NFT_Name = Transfer.contractInfo.name;
				NftTxn.nft.NFT_Short_Name = Transfer.contractInfo.symbol;
				NftTxn.nft.NFT_Details.token_id = FString::FromInt(Transfer.tokenIds[0]); // assume int conversion?
				NftTxn.nft.NFT_Details.Contract_Address = Transfer.contractInfo.address;
				NftTxn.nft.NFT_Details.Token_Standard = Transfer.contractType;

				Item.txn_history_nfts.Add(NftTxn);
			}
		}
		
		Array.Push(Item);
	}

	OnSuccess(Array);
}

FString UIndexerSupport::jsonToParsableString(TSharedPtr<FJsonValue> jsonData)
{
	return simplifyStringParsable(jsonToString(jsonData));
}

FString UIndexerSupport::jsonToParsableString(TSharedPtr<FJsonObject> jsonData)
{
	return simplifyStringParsable(jsonToString(jsonData));
}

/*
* This will convert a json value to an FString!
*/
FString UIndexerSupport::jsonToString(TSharedPtr<FJsonValue> jsonData)
{
	FString ret;
	TSharedRef< TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ret);
	FJsonSerializer::Serialize(jsonData.ToSharedRef(),"", Writer);
	return ret;
}

FString UIndexerSupport::stringCleanup(FString string)
{
	FString *ret = &string;

	//For some reason FJsonValues when they get parsed will have leading commas!
	if (string[0] == ',')
	{
		string.RemoveAt(0);//remove the initial character if it's a treasonous comma!
	}

	FString srch_n = TEXT("\n");//we want no returns the UI will take of this for us!
	FString srch_r = TEXT("\r");
	FString srch_t = TEXT("\t");
	FString srch_c = TEXT("\"");//this will be apart of json formatting get rid of it!
	FString replace = TEXT("");

	const TCHAR* srch_ptr_n = *srch_n;
	const TCHAR* srch_ptr_r = *srch_r;
	const TCHAR* srch_ptr_t = *srch_t;
	const TCHAR* srch_ptr_c = *srch_c;
	const TCHAR* rep_ptr = *replace;

	(*ret).ReplaceInline(srch_ptr_n, rep_ptr, ESearchCase::IgnoreCase);//remove \n
	(*ret).ReplaceInline(srch_ptr_r, rep_ptr, ESearchCase::IgnoreCase);//remove \r
	(*ret).ReplaceInline(srch_ptr_t, rep_ptr, ESearchCase::IgnoreCase);//remove \t
	(*ret).ReplaceInline(srch_ptr_c, rep_ptr, ESearchCase::IgnoreCase);//remove \"

	return (*ret);
}

FMonthDayYear_BE UIndexerSupport::TimestampToMonthDayYear_Be(FString Timestamp)
{
	return FMonthDayYear_BE{}; // TODO: Date conversion
}

//indexer response extractors

FUpdatableItemDataArgs UIndexerSupport::extractFromTokenBalances(FGetTokenBalancesReturn tokenBalances)
{
	FUpdatableItemDataArgs ret;
	
	for (FTokenBalance token : tokenBalances.balances)
	{
		if (token.contractType == EContractType::ERC1155 || token.contractType == EContractType::ERC1155_BRIDGE)
		{//NFT
			FNFT_BE nft;
			nft.NFT_Name = token.tokenMetaData.name;
			nft.NFT_Short_Name = token.tokenMetaData.name;
			nft.Collection_Long_Name = token.contractInfo.name;
			nft.Collection_Short_Name = token.contractInfo.symbol;
			nft.Description = token.tokenMetaData.description;
			nft.Properties = token.tokenMetaData.properties;
			nft.Amount = -1;//need an amount parser
			nft.Value = -1;
			nft.NFT_Icon_URL = token.tokenMetaData.image;
			nft.Collection_Icon_URL = token.contractInfo.extensions.ogImage;
			//parse nft details data
			nft.NFT_Details.External_URL = token.tokenMetaData.external_url;
			nft.NFT_Details.token_id.Append(FString::Printf(TEXT("%lld"), token.tokenID));
			nft.NFT_Details.Contract_Address = token.contractAddress;
			nft.NFT_Details.itemID.contractAddress = nft.NFT_Details.Contract_Address;
			nft.NFT_Details.itemID.chainID = token.chainId;
			nft.NFT_Details.Token_Standard = token.contractType;
			//not sure where or what we do with network icon or name though!
			//nft.NFT_Details.Network = "n/a";
			//nft.NFT_Details.Network_Icon = nullptr;
			ret.semiParsedBalances.nfts.Add(nft);

			FNFTUpdatable uNFT;//prep the updatable
			uNFT.nftCollectionIconUrl = token.contractInfo.extensions.ogImage;
			uNFT.nftIconUrl = token.tokenMetaData.image;
			uNFT.nftID = nft.NFT_Details.itemID;
			ret.updatingNftData.Add(uNFT);
		}
		else if (token.contractType == EContractType::ERC721 || token.contractType == EContractType::ERC721_BRIDGE || token.contractType == EContractType::ERC20 || token.contractType == EContractType::ERC20_BRIDGE)
		{//coin
			FCoin_BE coin;
			coin.Coin_Long_Name = token.contractInfo.name;
			coin.Coin_Short_Name = token.contractInfo.symbol;
			coin.Coin_Standard = token.contractType;
			coin.itemID.chainID = token.chainId;
			coin.itemID.contractAddress = token.contractAddress;
			coin.Coin_Amount = -1;//need an amount parser
			coin.Coin_Value = -1;
			coin.Coin_Symbol_URL = token.contractInfo.logoURI;
			ret.semiParsedBalances.coins.Add(coin);//add the semi parsed coin data

			FCoinUpdatable uCoin;//add the needed info for getting the rest of the coin data
			uCoin.coinIconUrl = token.contractInfo.logoURI;
			uCoin.coinID = coin.itemID;
			ret.updatingCoinData.Add(uCoin);
		}
	}
	return ret;
}