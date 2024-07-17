// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Containers/Union.h"
#include "Types/Types.h"
#include "Util/Structs/BE_Structs.h"
#include "IndexerSupport.generated.h"

struct FGetTokenBalancesReturn;
struct FTokenBalanceExtractorReturn;
struct FGetTransactionHistoryReturn;
struct FUpdatableItemDataArgs;

/**
 * Support class for the indexer
 */
UCLASS()
class SEQUENCEPLUGIN_API UIndexerSupport : public UObject
{
	GENERATED_BODY()
public:
	static float GetAmount(int64 Amount,int32 Decimals);
	static float GetAmount(int64 Amount,float Decimals);
	
	static FString TransactionListToJsonString(const TArray<TransactionUnion>& Transactions);
	
	/*
	* Used to convert a jsonObject into a hash map of FStrings
	*/
	static TMap<FString, FString> JSONObjectParser(TSharedPtr<FJsonObject> JSONData);

	static TSharedPtr<FJsonObject> JsonStringToObject(const FString& JSON);
	
	/*
	* Used to remove
	* \n, \", \t, \r, spaces etc
	*/
	static FString SimplifyString(FString String);

	static FString PartialSimpleString(FString String);

	/*
	* Similar to simplify string EXCEPT we keept he /" because we are trying
	* to maintain json correctness for parsing!
	*/
	static FString SimplifyStringParsable(FString String);

	/*
	* Allows for the converting of a Struct straigt into a nicely formatted json string!
	*/
	template < typename T > static FString StructToString(T StructVar)
	{
		FString Ret;
		FJsonObjectConverter::UStructToJsonObjectString<T>(StructVar, Ret, 0, 0);
		return Ret;
	}

	/*
	* Allows for the converting of a Struct straigt into a nicely formatted json string!
	* This also cleans up the string from json formatting in the event we just want an
	* easily testable string!
	*/
	template < typename T > static FString StructToSimpleString(T StructVar)
	{
		FString Ret;
		FJsonObjectConverter::UStructToJsonObjectString<T>(StructVar, Ret, 0, 0);
		Ret = SimplifyString(Ret);
		return Ret;
	}

	template < typename T > static FString StructToPartialSimpleString(T StructVar)
	{
		FString Ret;
		FJsonObjectConverter::UStructToJsonObjectString<T>(StructVar, Ret, 0, 0);
		Ret = PartialSimpleString(Ret);
		return Ret;
	}

	template < typename T > static T JSONStringToStruct(FString JSON)
	{
		T Ret;

		TSharedPtr<FJsonObject> JsonObj = MakeShareable<FJsonObject>(new FJsonObject);

		if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JSON), JsonObj))
		{
			if (!FJsonObjectConverter::JsonObjectToUStruct<T>(JsonObj.ToSharedRef(), &Ret))
				UE_LOG(LogTemp, Error, TEXT("[Failed to convert jsonObject into a UStruct: [%s]]"), *JSON);
		}
		else
		{//failed to convert the decrypted string into a jsonObject
			UE_LOG(LogTemp, Error, TEXT("[Failed to convert jsonObjectString into a jsonObject: [%s]]"), *JSON);
		}

		return Ret;
	}

	template < typename T > static bool JSONStringToStruct(FString JSON, T * PtrRet)
	{
		T TStruct;
		bool Ret = false;
		TSharedPtr<FJsonObject> JsonObj = MakeShareable<FJsonObject>(new FJsonObject);

		if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JSON), JsonObj))
		{
			if (FJsonObjectConverter::JsonObjectToUStruct<T>(JsonObj.ToSharedRef(), &TStruct))
			{
				*PtrRet = TStruct;
				Ret = true;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[Failed to convert jsonObject into a UStruct: [%s]]"), *JSON);
			}
		}
		else
		{//failed to convert the decrypted string into a jsonObject
			UE_LOG(LogTemp, Error, TEXT("[Failed to convert jsonObjectString into a jsonObject: [%s]]"), *JSON);
		}

		return Ret;
	}

	static FString StringListToSimpleString(TArray<FString> StringData);

	//for maintaining valid json for args in RPC calls
	static FString StringListToParsableString(TArray<FString> StringData);

	static FString Int64ListToSimpleString(TArray<int64> IntData);

	static FString JsonObjListToString(TArray<TSharedPtr<FJsonObject>> JsonData);
	
	static FString JsonObjListToSimpleString(TArray<TSharedPtr<FJsonObject>> JsonData);

	static FString JsonObjListToParsableString(TArray<TSharedPtr<FJsonObject>> JsonData);

	static FString JsonValueListToParsableString(TArray<TSharedPtr<FJsonValue>> JsonData);

	static FString JsonToString(TSharedPtr<FJsonValue> JsonData);

	static FString JsonToSimpleString(TSharedPtr<FJsonValue> JsonData);

	static FString JsonToParsableString(TSharedPtr<FJsonValue> JsonData);

	static FString JsonToParsableString(TSharedPtr<FJsonObject> JsonData);

	static FString JsonToString(TSharedPtr<FJsonObject> JsonData);

	static FString JsonToSimpleString(TSharedPtr<FJsonObject> JsonData);

	//indexer reponse extraction functions used to parse response into a frontend usable form!
	static FUpdatableHistoryArgs ExtractFromTransactionHistory(FString MyAddress, FGetTransactionHistoryReturn TransactionHistory);
	static FUpdatableItemDataArgs ExtractFromTokenBalances(FGetTokenBalancesReturn TokenBalances);

private:
	/*
	* Similar to simplifyString EXCEPT we also cleanup 
	* some special edge cases from json responses / parsing as well!
	*/
	static FString StringCleanup(FString String);

	static FMonthDayYear_BE TimestampToMonthDayYear_Be(FString Timestamp);
};
