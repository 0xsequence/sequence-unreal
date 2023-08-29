// Fill out your copyright notice in the Description page of Project Settings.

#include "Indexer/IndexerSupport.h"
#include "Indexer/Structs/Version.h"
#include "Indexer/Structs/GetTransactionHistoryReturn.h"
#include "Indexer/Structs/Struct_Data.h"
#include "Util/Structs/BE_Structs.h"
#include "Indexer/Indexer.h"

float UIndexerSupport::getAmount(int64 amount, int32 decimals)
{
	float ret = amount;

	ret /= FMath::Pow(10,float(decimals));

	return ret;
}
float UIndexerSupport::getAmount(int64 amount, float decimals)
{
	float ret = amount;

	ret /= FMath::Pow(10, decimals);

	return ret;
}

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

FUpdatableHistoryArgs UIndexerSupport::extractFromTransactionHistory(FString MyAddress, FGetTransactionHistoryReturn TransactionHistory)
{
	FUpdatableHistoryArgs UpdateItems;

	for(FTransaction Transaction : TransactionHistory.transactions)
	{
		FTransactionHistoryItem_BE Item;
		Item.network_name = *UIndexer::GetIndexerNames().Find(Transaction.chainId);
		Item.network_icon = nullptr;
		bool TxnTypeSet = false;
		
		for(FTxnTransfer Transfer : Transaction.transfers)
		{
			//preprocess all the indexable amounts and properties
			int64 TokenId = -1;
			int64 amount = -1;

			if (Transfer.tokenIds.Num() > 0)
			{
				TokenId = Transfer.tokenIds[0];
			}

			if (Transfer.amounts.Num() > 0)
			{
				amount = Transfer.amounts[0];
			}

			bool hasMetaData = false;
			FTokenMetaData* TokenMetaData = nullptr;
			if (Transfer.tokenMetaData.Contains(FString::FromInt(TokenId)))
			{
				hasMetaData = true;
				TokenMetaData = Transfer.tokenMetaData.Find(FString::FromInt(TokenId));
			}
			//end of pre processing

			if (Transfer.contractType == EContractType::ERC1155 || Transfer.contractType == EContractType::ERC1155_BRIDGE)
			{//NFT
				FNFTTxn_BE NftTxn;
				if (hasMetaData)
				{
					TokenMetaData = Transfer.tokenMetaData.Find(FString::FromInt(TokenId));
					NftTxn.amount = UIndexerSupport::getAmount(amount, TokenMetaData->decimals);
					NftTxn.nft.NFT_Name = TokenMetaData->name;
					NftTxn.nft.NFT_Short_Name = TokenMetaData->name;
					NftTxn.nft.NFT_Icon_URL = TokenMetaData->image;
				}

				NftTxn.nft.Amount = NftTxn.amount;
				NftTxn.nft.Description = Transfer.contractInfo.extensions.description;
				NftTxn.nft.Value = -1.0; //we get this later
				NftTxn.nft.Collection_Long_Name = Transfer.contractInfo.name;
				NftTxn.nft.Collection_Short_Name = Transfer.contractInfo.symbol;
				NftTxn.nft.Collection_Icon_URL = Transfer.contractInfo.extensions.ogImage;
				//nft details
				NftTxn.nft.NFT_Details.token_id = FString::FromInt(TokenId);
				NftTxn.nft.NFT_Details.Contract_Address = Transfer.contractInfo.address;
				NftTxn.nft.NFT_Details.Token_Standard = Transfer.contractType;
				//item id
				NftTxn.nft.NFT_Details.itemID.contractAddress = NftTxn.nft.NFT_Details.Contract_Address;
				NftTxn.nft.NFT_Details.itemID.chainID = Transaction.chainId;

				//add to the list of history items
				Item.txn_history_nfts.Add(NftTxn);

				//prep the updatable
				FNFTUpdatable uNFT;
				uNFT.nftID = NftTxn.nft.NFT_Details.itemID;
				uNFT.nftCollectionIconUrl = NftTxn.nft.Collection_Icon_URL;
				uNFT.nftIconUrl = NftTxn.nft.NFT_Icon_URL;
				UpdateItems.updatingNftData.Add(uNFT);
			}
			else if (Transfer.contractType == EContractType::ERC721 || Transfer.contractType == EContractType::ERC721_BRIDGE || Transfer.contractType == EContractType::ERC20 || Transfer.contractType == EContractType::ERC20_BRIDGE)
			{//coin
				FCoinTxn_BE CoinTxn;
				CoinTxn.amount = UIndexerSupport::getAmount(amount,Transfer.contractInfo.decimals);
				CoinTxn.coin.Coin_Symbol_URL = Transfer.contractInfo.logoURI;
				CoinTxn.coin.Coin_Short_Name = Transfer.contractInfo.symbol;
				CoinTxn.coin.Coin_Long_Name = Transfer.contractInfo.name;
				CoinTxn.coin.Coin_Amount = CoinTxn.amount;
				CoinTxn.coin.Coin_Standard = Transfer.contractType;
				CoinTxn.coin.Coin_Value = -1.0; //we get this in the 2nd part of the level of updating

				//item id
				CoinTxn.coin.itemID.contractAddress = Transfer.contractInfo.address;
				CoinTxn.coin.itemID.chainID = Transaction.chainId;

				//Add it to the transaction history entry
				Item.txn_history_coins.Add(CoinTxn);

				//add the needed info for getting the rest of the coin data
				FCoinUpdatable uCoin;
				uCoin.coinIconUrl = Transfer.contractInfo.logoURI;
				uCoin.coinID = CoinTxn.coin.itemID;
				UpdateItems.updatingCoinData.Add(uCoin);
			}

			if(!TokenMetaData)
			{
				FSequenceError(RequestFail, "No token metadata found!");
			}
			
			Item.other_public_address = Transfer.from == MyAddress ? Transfer.to : Transfer.from;

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
			
			//they use a very precise timestamp whereas our time format is simple
			Item.transaction_date = TimestampToMonthDayYear_Be(Transaction.timestamp);
		}
		
		UpdateItems.semiParsedHistory.Add(Item);
	}//for

	return UpdateItems;
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
	//format: 2023-08-21T15:06:09Z to 21,08,2023
	FMonthDayYear_BE date;
	int32 loc = Timestamp.Find("T", ESearchCase::CaseSensitive);
	FString dateString = Timestamp.Left(loc);
	//need to do 2 splits
	FString yearData, monthDayData, monthData, dayData;
	FString *yearString = &yearData, *monthDayString = &monthDayData,*monthString = &monthData,*dayString = &dayData;
	dateString.Split("-",yearString,monthDayString);
	monthDayString->Split("-",monthString,dayString);
	//now year is in yearString, month is in monthString & day is in dayString
	date.Day = FCString::Atoi(**dayString);
	date.Month = FCString::Atoi(**monthString);
	date.Year = FCString::Atoi(**yearString);
	return date;
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
			nft.Amount = UIndexerSupport::getAmount(token.balance,token.tokenMetaData.decimals);
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
			nft.NFT_Details.Network = UIndexer::GetIndexerName(token.chainId);
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
			coin.Coin_Amount = UIndexerSupport::getAmount(token.balance,token.contractInfo.decimals);
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