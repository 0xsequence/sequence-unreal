// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Util/SequenceSupport.h"
#include "Indexer/Structs/SeqGetTransactionHistoryReturn.h"
#include "Indexer/Structs/Struct_Data.h"
#include "Util/Structs/BE_Structs.h"
#include "Indexer/Indexer.h"
#include "Misc/Base64.h"
#include "Types/BinaryData.h"

FString USequenceSupport::GetNetworkName(const int64 NetworkIdIn)
{
	if (NetworkIdToNameMap.Contains(NetworkIdIn))
	{
		return *NetworkIdToNameMap.Find(NetworkIdIn);
	}
	UE_LOG(LogTemp, Warning, TEXT("Name not found for Id: %lld"), NetworkIdIn);
	return TEXT("");
}

int64 USequenceSupport::GetNetworkId(const FString& NetworkNameIn)
{
	FString SearchKey = NetworkNameIn.ToLower();
	SearchKey.RemoveSpacesInline();
	if (NetworkNameToIdMap.Contains(SearchKey))
	{
		return *NetworkNameToIdMap.Find(SearchKey);
	}
	UE_LOG(LogTemp, Warning, TEXT("Id not found for Name: %s"), *NetworkNameIn);
	return -1;
}

TArray<FIdName> USequenceSupport::GetAllNetworks()
{
	return NetworkIdToNameMap.Array();
}

TArray<FIdNamePair> USequenceSupport::GetAllNetworksAsStructs()
{
	TArray<FIdNamePair> Networks;

	for (FIdName IdName : NetworkIdToNameMap.Array())
	{
		Networks.Add(FIdNamePair(IdName));
	}
	
	return Networks;
}

TArray<FString> USequenceSupport::GetAllNetworkNames()
{
	TArray<FString> NetworkNames;
	NetworkNameToIdMap.GetKeys(NetworkNames);
	return NetworkNames;
}

TArray<int64> USequenceSupport::GetAllNetworkIds()
{
	TArray<int64> NetworkIds;
	NetworkIdToNameMap.GetKeys(NetworkIds);
	return NetworkIds;
}

float USequenceSupport::GetAmount(const int64 Amount, const int64 Decimals)
{
	const float Ret = static_cast<float>(Amount);
	return static_cast<float>(Ret / FMath::Pow(10, static_cast<double>(Decimals)));
}

int64 USequenceSupport::GetAmount(const float Amount, const int64 Decimals)
{
	const double Operand = FMath::Pow(10, static_cast<double>(Decimals));
	const int64 Ret = static_cast<int64>(Operand * Amount);
	return Ret;
}

FString USequenceSupport::GetStringFromToken(const FString& IdToken, const FString& ParameterName)
{
	TArray<FString> B64Json;
	IdToken.ParseIntoArray(B64Json, TEXT("."), true);
	
	for (int i = 0; i < B64Json.Num(); i++)
	{
		FString T;
		FBase64::Decode(B64Json[i], T);
		B64Json[i] = T;
	}

	for (auto Part : B64Json)
	{
		const TSharedPtr<FJsonObject> PartJson = JsonStringToObject(Part);
		FString Parameter;
		if (PartJson.Get()->TryGetStringField(ParameterName,Parameter))
		{
			return Parameter;
		}
	}	
	return "";
}

int32 USequenceSupport::GetInt32FromToken(const FString& IdToken, const FString& ParameterName)
{
	TArray<FString> B64Json;
	IdToken.ParseIntoArray(B64Json, TEXT("."), true);
	
	for (int i = 0; i < B64Json.Num(); i++)
	{
		FString T;
		FBase64::Decode(B64Json[i], T);
		B64Json[i] = T;
	}

	for (auto Part : B64Json)
	{
		const TSharedPtr<FJsonObject> PartJson = JsonStringToObject(Part);
		int32 Parameter;
		if (PartJson.Get()->TryGetNumberField(ParameterName,Parameter))
		{
			return Parameter;
		}
	}	
	return -1;
}

int64 USequenceSupport::GetInt64FromToken(const FString& IdToken, const FString& ParameterName)
{
	TArray<FString> B64Json;
	IdToken.ParseIntoArray(B64Json, TEXT("."), true);
	
	for (int i = 0; i < B64Json.Num(); i++)
	{
		FString T;
		FBase64::Decode(B64Json[i], T);
		B64Json[i] = T;
	}

	for (auto Part : B64Json)
	{
		const TSharedPtr<FJsonObject> PartJson = JsonStringToObject(Part);
		int64 Parameter;
		if (PartJson.Get()->TryGetNumberField(ParameterName,Parameter))
		{
			return Parameter;
		}
	}	
	return -1;
}

float USequenceSupport::GetFloatFromToken(const FString& IdToken, const FString& ParameterName)
{
	TArray<FString> B64Json;
	IdToken.ParseIntoArray(B64Json, TEXT("."), true);
	
	for (int i = 0; i < B64Json.Num(); i++)
	{
		FString T;
		FBase64::Decode(B64Json[i], T);
		B64Json[i] = T;
	}

	for (auto Part : B64Json)
	{
		const TSharedPtr<FJsonObject> PartJson = JsonStringToObject(Part);
		float Parameter;
		if (PartJson.Get()->TryGetNumberField(ParameterName,Parameter))
		{
			return Parameter;
		}
	}	
	return -1;
}

double USequenceSupport::GetDoubleFromToken(const FString& IdToken, const FString& ParameterName)
{
	TArray<FString> B64Json;
	IdToken.ParseIntoArray(B64Json, TEXT("."), true);
	
	for (int i = 0; i < B64Json.Num(); i++)
	{
		FString T;
		FBase64::Decode(B64Json[i], T);
		B64Json[i] = T;
	}

	for (auto Part : B64Json)
	{
		const TSharedPtr<FJsonObject> PartJson = JsonStringToObject(Part);
		double Parameter;
		if (PartJson.Get()->TryGetNumberField(ParameterName,Parameter))
		{
			return Parameter;
		}
	}	
	return -1;
}

bool USequenceSupport::GetBoolFromToken(const FString& IdToken, const FString& ParameterName)
{
	TArray<FString> B64Json;
	IdToken.ParseIntoArray(B64Json, TEXT("."), true);
	
	for (int i = 0; i < B64Json.Num(); i++)
	{
		FString T;
		FBase64::Decode(B64Json[i], T);
		B64Json[i] = T;
	}

	for (auto Part : B64Json)
	{
		const TSharedPtr<FJsonObject> PartJson = JsonStringToObject(Part);
		bool Parameter;
		if (PartJson.Get()->TryGetBoolField(ParameterName,Parameter))
		{
			return Parameter;
		}
	}	
	return false;
}

FString USequenceSupport::TransactionListToJsonString(const TArray<TransactionUnion>& Transactions)
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
TMap<FString, FString> USequenceSupport::JSONObjectParser(const TSharedPtr<FJsonObject>& JSONData)
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

TSharedPtr<FJsonObject> USequenceSupport::JsonStringToObject(const FString& JSON)
{
	TSharedPtr<FJsonObject> Ret;
	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JSON), Ret))
		return Ret;
	else
		return nullptr;
}

FString USequenceSupport::PartialSimpleString(FString String)
{
	FString* Ret = &String;

	const FString Search_N = TEXT("\n");//we want no returns the UI will take of this for us!
	const FString Search_R = TEXT("\r");
	const FString Search_T = TEXT("\t");
	const FString Replace = TEXT("");

	const TCHAR* Search_Ptr_N = *Search_N;
	const TCHAR* Search_Ptr_R = *Search_R;
	const TCHAR* Search_Ptr_T = *Search_T;
	const TCHAR* Rep_Ptr = *Replace;

	Ret->RemoveSpacesInline();//remove spaces
	Ret->ReplaceInline(Search_Ptr_N, Rep_Ptr, ESearchCase::IgnoreCase);//remove \n
	Ret->ReplaceInline(Search_Ptr_R, Rep_Ptr, ESearchCase::IgnoreCase);//remove \r
	Ret->ReplaceInline(Search_Ptr_T, Rep_Ptr, ESearchCase::IgnoreCase);//remove \t

	return (*Ret);
}

FString USequenceSupport::SimplifyString(FString String)
{
	FString* Ret = &String;

	const FString Search_N = TEXT("\n");//we want no returns the UI will take of this for us!
	const FString Search_R = TEXT("\r");
	const FString Search_T = TEXT("\t");
	const FString Search_C = TEXT("\"");//this will be apart of json formatting get rid of it!
	const FString Replace = TEXT("");

	const TCHAR* Search_Ptr_N = *Search_N;
	const TCHAR* Search_Ptr_R = *Search_R;
	const TCHAR* Search_Ptr_T = *Search_T;
	const TCHAR* Search_Ptr_C = *Search_C;
	const TCHAR* Rep_Ptr = *Replace;

	Ret->RemoveSpacesInline();//remove spaces
	Ret->ReplaceInline(Search_Ptr_N, Rep_Ptr, ESearchCase::IgnoreCase);//remove \n
	Ret->ReplaceInline(Search_Ptr_R, Rep_Ptr, ESearchCase::IgnoreCase);//remove \r
	Ret->ReplaceInline(Search_Ptr_T, Rep_Ptr, ESearchCase::IgnoreCase);//remove \t
	Ret->ReplaceInline(Search_Ptr_C, Rep_Ptr, ESearchCase::IgnoreCase);//remove \"

	return (*Ret);
}

FString USequenceSupport::SimplifyStringParsable(FString String)
{
	FString* Ret = &String;

	const FString Search_N = TEXT("\n");//we want no returns the UI will take of this for us!
	const FString Search_R = TEXT("\r");
	const FString Search_T = TEXT("\t");
	const FString Replace = TEXT("");

	const TCHAR* Search_Ptr_N = *Search_N;
	const TCHAR* Search_Ptr_R = *Search_R;
	const TCHAR* Search_Ptr_T = *Search_T;
	const TCHAR* Rep_Ptr = *Replace;

	Ret->RemoveSpacesInline();//remove spaces
	Ret->ReplaceInline(Search_Ptr_N, Rep_Ptr, ESearchCase::IgnoreCase);//remove \n
	Ret->ReplaceInline(Search_Ptr_R, Rep_Ptr, ESearchCase::IgnoreCase);//remove \r
	Ret->ReplaceInline(Search_Ptr_T, Rep_Ptr, ESearchCase::IgnoreCase);//remove \t

	return (*Ret);
}

FString USequenceSupport::StringListToSimpleString(TArray<FString> StringData)
{
	FString Ret = "[";

	for (FString string : StringData)
	{
		Ret += string;
		Ret += ",";
	}

	if (StringData.Num() > 0)
	{
		Ret.RemoveAt(Ret.Len() - 1);//remove the last comma as it'll be wrong!
	}
	Ret += "]";
	return Ret;
}

FString USequenceSupport::StringListToParsableString(TArray<FString> StringData)
{
	FString Ret = "[";
	for (FString StringDatum : StringData)
	{
		Ret += "\""+StringDatum+"\"";
		Ret += ",";
	}

	if (StringData.Num() > 0)
	{
		Ret.RemoveAt(Ret.Len() - 1);//remove the last comma as it'll be wrong!
	}
	Ret += "]";
	return Ret;
}

FString USequenceSupport::Int64ListToSimpleString(TArray<int64> IntData)
{
	FString Ret = "[";

	for (int64 IData : IntData)
	{
		FString IDataString = FString::Printf(TEXT("%lld"), IData);
		Ret += IDataString + ",";
	}

	if (IntData.Num() > 0)
	{
		Ret.RemoveAt(Ret.Len() - 1);//remove the last comma as it'll be wrong!
	}

	Ret += "]";
	return Ret;
}

FString USequenceSupport::JsonObjListToString(TArray<TSharedPtr<FJsonObject>> JsonData)
{
	FString Ret = "[";
	for (TSharedPtr<FJsonObject> jObj : JsonData)
	{
		Ret.Append(USequenceSupport::JsonToString(jObj));
		Ret.Append(",");
	}

	if (JsonData.Num() > 0)
	{
		Ret.RemoveAt(Ret.Len() - 1);
	}
	Ret.Append("]");
	return Ret;
}

FString USequenceSupport::JsonObjListToSimpleString(TArray<TSharedPtr<FJsonObject>> JsonData)
{
	FString Ret = "[";
	for (TSharedPtr<FJsonObject> jObj : JsonData)
	{
		Ret.Append(USequenceSupport::JsonToSimpleString(jObj));
		Ret.Append(",");
	}

	if (JsonData.Num() > 0)
	{
		Ret.RemoveAt(Ret.Len() - 1);
	}
	Ret.Append("]");
	return Ret;
}

FString USequenceSupport::JsonObjListToParsableString(TArray<TSharedPtr<FJsonObject>> JsonData)
{
	FString Ret = "[";
	for (TSharedPtr<FJsonObject> jObj : JsonData)
	{
		Ret.Append(USequenceSupport::JsonToParsableString(jObj));
		Ret.Append(",");
	}

	if (JsonData.Num() > 0)
	{
		Ret.RemoveAt(Ret.Len() - 1);
	}
	Ret.Append("]");
	return Ret;
}

FString USequenceSupport::JsonValueListToParsableString(const TArray<TSharedPtr<FJsonValue>>& JsonData)
{
	FString JsonList;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonList);
	FJsonSerializer::Serialize(JsonData, Writer);
	return SimplifyStringParsable(JsonList);
}

FString USequenceSupport::JsonToString(const TSharedPtr<FJsonObject>& JsonData)
{
	FString ret;
	const TSharedRef< TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ret);
	FJsonSerializer::Serialize(JsonData.ToSharedRef(), Writer);
	return ret;
}

FString USequenceSupport::JsonToSimpleString(const TSharedPtr<FJsonValue>& JsonData)
{
	return SimplifyString(JsonToString(JsonData));
}

FString USequenceSupport::JsonToSimpleString(const TSharedPtr<FJsonObject>& JsonData)
{
	return SimplifyString(JsonToString(JsonData));
}

FUpdatableHistoryArgs USequenceSupport::ExtractFromTransactionHistory(FString MyAddress, FSeqGetTransactionHistoryReturn TransactionHistory)
{
	FUpdatableHistoryArgs UpdateItems;

	for(FSeqTransaction Transaction : TransactionHistory.transactions)
	{
		FTransactionHistoryItem_BE Item;
		Item.network_name = USequenceSupport::GetNetworkName(Transaction.chainId);
		Item.network_icon = nullptr;
		bool TxnTypeSet = false;
		
		for(FSeqTxnTransfer Transfer : Transaction.transfers)
		{
			//preprocess all the Indexed amounts and properties
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
			FSeqTokenMetaData* TokenMetaData = nullptr;
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
					NftTxn.amount = USequenceSupport::GetAmount(amount, TokenMetaData->decimals);
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
				CoinTxn.amount = USequenceSupport::GetAmount(amount,Transfer.contractInfo.decimals);
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

FString USequenceSupport::JsonToParsableString(const TSharedPtr<FJsonValue>& JsonData)
{
	return SimplifyStringParsable(JsonToString(JsonData));
}

FString USequenceSupport::JsonToParsableString(const TSharedPtr<FJsonObject>& JsonData)
{
	return SimplifyStringParsable(JsonToString(JsonData));
}

/*
* This will convert a json value to an FString!
*/
FString USequenceSupport::JsonToString(const TSharedPtr<FJsonValue>& JsonData)
{
	FString ret;
	const TSharedRef< TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ret);
	FJsonSerializer::Serialize(JsonData.ToSharedRef(),"", Writer);
	return ret;
}

FString USequenceSupport::StringCleanup(FString String)
{
	FString *Ret = &String;

	//For some reason FJsonValues when they get parsed will have leading commas!
	if (String[0] == ',')
	{
		String.RemoveAt(0);//remove the initial character if it's a treasonous comma!
	}

	const FString Search_N = TEXT("\n");//we want no returns the UI will take of this for us!
	const FString Search_R = TEXT("\r");
	const FString Search_T = TEXT("\t");
	const FString Search_C = TEXT("\"");//this will be apart of json formatting get rid of it!
	const FString Replace = TEXT("");

	const TCHAR* Search_Ptr_N = *Search_N;
	const TCHAR* Search_Ptr_R = *Search_R;
	const TCHAR* Search_Ptr_T = *Search_T;
	const TCHAR* Search_Ptr_C = *Search_C;
	const TCHAR* Rep_Ptr = *Replace;

	Ret->ReplaceInline(Search_Ptr_N, Rep_Ptr, ESearchCase::IgnoreCase);//remove \n
	Ret->ReplaceInline(Search_Ptr_R, Rep_Ptr, ESearchCase::IgnoreCase);//remove \r
	Ret->ReplaceInline(Search_Ptr_T, Rep_Ptr, ESearchCase::IgnoreCase);//remove \t
	Ret->ReplaceInline(Search_Ptr_C, Rep_Ptr, ESearchCase::IgnoreCase);//remove \"

	return (*Ret);
}

FMonthDayYear_BE USequenceSupport::TimestampToMonthDayYear_Be(const FString& Timestamp)
{
	//format: 2023-08-21T15:06:09Z to 21,08,2023
	FMonthDayYear_BE Date;
	const int32 Loc = Timestamp.Find("T", ESearchCase::CaseSensitive);
	const FString DateString = Timestamp.Left(Loc);
	//need to do 2 splits
	FString YearData, MonthDayData, MonthData, DayData;
	FString *YearString = &YearData, *MonthDayString = &MonthDayData,*MonthString = &MonthData,*DayString = &DayData;
	DateString.Split("-",YearString,MonthDayString);
	MonthDayString->Split("-",MonthString,DayString);
	//now year is in yearString, month is in monthString & day is in dayString
	Date.Day = FCString::Atoi(**DayString);
	Date.Month = FCString::Atoi(**MonthString);
	Date.Year = FCString::Atoi(**YearString);
	return Date;
}

int64 USequenceSupport::StringDateToUnixDate(const FString& Iso8601)
{
	FDateTime ParsedDate;
	FDateTime::ParseIso8601(*Iso8601,ParsedDate);
	return ParsedDate.ToUnixTimestamp();
}

//indexer response extractors

FUpdatableItemDataArgs USequenceSupport::ExtractFromTokenBalances(FSeqGetTokenBalancesReturn TokenBalances)
{
	FUpdatableItemDataArgs ret;
	
	for (FSeqTokenBalance token : TokenBalances.balances)
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
			nft.Amount = USequenceSupport::GetAmount(token.balance,token.tokenMetaData.decimals);
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
			nft.NFT_Details.Network = USequenceSupport::GetNetworkName(token.chainId);
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
			coin.Coin_Amount = USequenceSupport::GetAmount(token.balance,token.contractInfo.decimals);
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