// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Indexer/IndexerSupport.h"
#include "Indexer/Structs/Version.h"
#include "Indexer/Structs/GetTransactionHistoryReturn.h"
#include "Indexer/Structs/Struct_Data.h"
#include "Util/Structs/BE_Structs.h"
#include "Indexer/Indexer.h"
#include "Logging/StructuredLogFormat.h"

float UIndexerSupport::GetAmount(const int64 Amount, const int64 Decimals)
{
	const float Ret = static_cast<float>(Amount);
	return static_cast<float>(Ret / FMath::Pow(10, static_cast<double>(Decimals)));
}

int64 UIndexerSupport::GetAmount(const float Amount, const int64 Decimals)
{
	const double Operand = FMath::Pow(10, static_cast<double>(Decimals));
	const int64 Ret = static_cast<int64>(Operand * Amount);
	return Ret;
}

FString UIndexerSupport::TransactionListToJsonString(const TArray<TransactionUnion>& Transactions)
{
	FString TransactionsPayload = "[";
	
	for (TransactionUnion Transaction : Transactions)
	{
		switch(Transaction.GetCurrentSubtypeIndex())
		{
		case 0: //RawTransaction
			TransactionsPayload += Transaction.GetSubtype<FRawTransaction>().GetJsonString() + ",";
			break;
		case 1: //ERC20
			TransactionsPayload += Transaction.GetSubtype<FERC20Transaction>().GetJsonString() + ",";
			break;
		case 2: //ERC721
			TransactionsPayload += Transaction.GetSubtype<FERC721Transaction>().GetJsonString() + ",";
			break;
		case 3: //ERC1155
			TransactionsPayload += Transaction.GetSubtype<FERC1155Transaction>().GetJsonString() + ",";
			break;
		case 4: //DelayedEncoding
			TransactionsPayload += Transaction.GetSubtype<FDelayedTransaction>().GetJsonString() + ",";
			break;
		default: //Doesn't match
			break;
		}
	}
	if (TransactionsPayload.Len() > 1)
	{
		TransactionsPayload.RemoveAt(TransactionsPayload.Len() - 1);
	}
	TransactionsPayload += "]";
	return TransactionsPayload;
}

/*
* This will convert a jsonObject into a TMap<FString,FString> thereby making a dynamic
* object usable in the UI!
*/
TMap<FString, FString> UIndexerSupport::JSONObjectParser(TSharedPtr<FJsonObject> JSONData)
{
	TArray<TPair<FString, TSharedPtr<FJsonValue>>> jsonList = JSONData.Get()->Values.Array();
	TMap<FString, FString> ret;
	ret.Reserve(jsonList.Num());//put some slack in to speed this up!

	for (TPair<FString,TSharedPtr<FJsonValue>> kvp : jsonList)
	{
		TPair<FString, FString> tPair(kvp.Key, StringCleanup(JsonToString(kvp.Value)));
		ret.Add(tPair);
	}
	return ret;
}

TSharedPtr<FJsonObject> UIndexerSupport::JsonStringToObject(const FString& JSON)
{
	TSharedPtr<FJsonObject> Ret;
	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JSON), Ret))
		return Ret;
	else
		return nullptr;
}

FString UIndexerSupport::PartialSimpleString(FString String)
{
	FString* ret = &String;

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

FString UIndexerSupport::SimplifyString(FString String)
{
	FString* ret = &String;

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

FString UIndexerSupport::SimplifyStringParsable(FString String)
{
	FString* ret = &String;

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

FString UIndexerSupport::StringListToSimpleString(TArray<FString> StringData)
{
	FString ret = "[";

	for (FString string : StringData)
	{
		ret += string;
		ret += ",";
	}

	if (StringData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);//remove the last comma as it'll be wrong!
	}
	ret += "]";
	return ret;
}

FString UIndexerSupport::StringListToParsableString(TArray<FString> StringData)
{
	FString ret = "[";
	for (FString string : StringData)
	{
		ret += "\""+string+"\"";
		ret += ",";
	}

	if (StringData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);//remove the last comma as it'll be wrong!
	}
	ret += "]";
	return ret;
}

FString UIndexerSupport::Int64ListToSimpleString(TArray<int64> IntData)
{
	FString ret = "[";

	for (int64 iData : IntData)
	{
		FString iDataString = FString::Printf(TEXT("%lld"), iData);
		ret += iDataString + ",";
	}

	if (IntData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);//remove the last comma as it'll be wrong!
	}

	ret += "]";
	return ret;
}

FString UIndexerSupport::JsonObjListToString(TArray<TSharedPtr<FJsonObject>> JsonData)
{
	FString ret = "[";
	for (TSharedPtr<FJsonObject> jObj : JsonData)
	{
		ret.Append(UIndexerSupport::JsonToString(jObj));
		ret.Append(",");
	}

	if (JsonData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);
	}
	ret.Append("]");
	return ret;
}

FString UIndexerSupport::JsonObjListToSimpleString(TArray<TSharedPtr<FJsonObject>> JsonData)
{
	FString ret = "[";
	for (TSharedPtr<FJsonObject> jObj : JsonData)
	{
		ret.Append(UIndexerSupport::JsonToSimpleString(jObj));
		ret.Append(",");
	}

	if (JsonData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);
	}
	ret.Append("]");
	return ret;
}

FString UIndexerSupport::JsonObjListToParsableString(TArray<TSharedPtr<FJsonObject>> JsonData)
{
	FString ret = "[";
	for (TSharedPtr<FJsonObject> jObj : JsonData)
	{
		ret.Append(UIndexerSupport::JsonToParsableString(jObj));
		ret.Append(",");
	}

	if (JsonData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);
	}
	ret.Append("]");
	return ret;
}

FString UIndexerSupport::JsonValueListToParsableString(TArray<TSharedPtr<FJsonValue>> JsonData)
{
	FString JsonList;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonList);
	FJsonSerializer::Serialize(JsonData, Writer);
	return SimplifyStringParsable(JsonList);
}

FString UIndexerSupport::JsonToString(TSharedPtr<FJsonObject> JsonData)
{
	FString ret;
	TSharedRef< TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ret);
	FJsonSerializer::Serialize(JsonData.ToSharedRef(), Writer);
	return ret;
}

FString UIndexerSupport::JsonToSimpleString(TSharedPtr<FJsonValue> JsonData)
{
	return SimplifyString(JsonToString(JsonData));
}

FString UIndexerSupport::JsonToSimpleString(TSharedPtr<FJsonObject> JsonData)
{
	return SimplifyString(JsonToString(JsonData));
}

FUpdatableHistoryArgs UIndexerSupport::ExtractFromTransactionHistory(FString MyAddress, FGetTransactionHistoryReturn TransactionHistory)
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
					NftTxn.amount = UIndexerSupport::GetAmount(amount, TokenMetaData->decimals);
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
				CoinTxn.amount = UIndexerSupport::GetAmount(amount,Transfer.contractInfo.decimals);
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

FString UIndexerSupport::JsonToParsableString(TSharedPtr<FJsonValue> JsonData)
{
	return SimplifyStringParsable(JsonToString(JsonData));
}

FString UIndexerSupport::JsonToParsableString(TSharedPtr<FJsonObject> JsonData)
{
	return SimplifyStringParsable(JsonToString(JsonData));
}

/*
* This will convert a json value to an FString!
*/
FString UIndexerSupport::JsonToString(TSharedPtr<FJsonValue> JsonData)
{
	FString ret;
	TSharedRef< TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ret);
	FJsonSerializer::Serialize(JsonData.ToSharedRef(),"", Writer);
	return ret;
}

FString UIndexerSupport::StringCleanup(FString String)
{
	FString *ret = &String;

	//For some reason FJsonValues when they get parsed will have leading commas!
	if (String[0] == ',')
	{
		String.RemoveAt(0);//remove the initial character if it's a treasonous comma!
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

FUpdatableItemDataArgs UIndexerSupport::ExtractFromTokenBalances(FGetTokenBalancesReturn TokenBalances)
{
	FUpdatableItemDataArgs ret;
	
	for (FTokenBalance token : TokenBalances.balances)
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
			nft.Amount = UIndexerSupport::GetAmount(token.balance,token.tokenMetaData.decimals);
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
			coin.Coin_Amount = UIndexerSupport::GetAmount(token.balance,token.contractInfo.decimals);
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