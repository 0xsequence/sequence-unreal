// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "JsonObjectConverter.h"
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
class UIndexerSupport : public UObject
{
	GENERATED_BODY()
public:
	static float getAmount(int64 amount,int32 decimals);
	static float getAmount(int64 amount,float decimals);
	/*
	* Used to convert a jsonObject into a hash map of FStrings
	*/
	static TMap<FString, FString> jsonObjectParser(TSharedPtr<FJsonObject> jsonData);

	/*
	* Used to remove
	* \n, \", \t, \r, spaces etc
	*/
	static FString simplifyString(FString string);

	/*
	* Similar to simplify string EXCEPT we keept he /" because we are trying
	* to maintain json correctness for parsing!
	*/
	static FString simplifyStringParsable(FString string);

	/*
	* Allows for the converting of a Struct straigt into a nicely formatted json string!
	*/
	template < typename T > static FString structToString(T structVar)
	{
		FString ret;
		FJsonObjectConverter::UStructToJsonObjectString<T>(structVar, ret, 0, 0);
		return ret;
	}

	/*
	* Allows for the converting of a Struct straigt into a nicely formatted json string!
	* This also cleans up the string from json formatting in the event we just want an
	* easily testable string!
	*/
	template < typename T > static FString structToSimpleString(T structVar)
	{
		FString ret;
		FJsonObjectConverter::UStructToJsonObjectString<T>(structVar, ret, 0, 0);
		ret = simplifyString(ret);
		return ret;
	}

	template < typename T > static T jsonStringToStruct(FString json)
	{
		T ret;

		TSharedPtr<FJsonObject> JsonObj = MakeShareable<FJsonObject>(new FJsonObject);

		if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(json), JsonObj))
		{
			if (!FJsonObjectConverter::JsonObjectToUStruct<T>(JsonObj.ToSharedRef(), &ret))
				UE_LOG(LogTemp, Error, TEXT("[Failed to convert jsonObject into a UStruct: [%s]]"), *json);
		}
		else
		{//failed to convert the decrypted string into a jsonObject
			UE_LOG(LogTemp, Error, TEXT("[Failed to convert jsonObjectString into a jsonObject: [%s]]"), *json);
		}

		return ret;
	}

	template < typename T > static bool jsonStringToStruct(FString json, T * ptrRet)
	{
		T TStruct;
		bool ret = false;
		TSharedPtr<FJsonObject> JsonObj = MakeShareable<FJsonObject>(new FJsonObject);

		if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(json), JsonObj))
		{
			if (FJsonObjectConverter::JsonObjectToUStruct<T>(JsonObj.ToSharedRef(), &TStruct))
			{
				*ptrRet = TStruct;
				ret = true;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[Failed to convert jsonObject into a UStruct: [%s]]"), *json);
			}
		}
		else
		{//failed to convert the decrypted string into a jsonObject
			UE_LOG(LogTemp, Error, TEXT("[Failed to convert jsonObjectString into a jsonObject: [%s]]"), *json);
		}

		return ret;
	}

	static FString stringListToSimpleString(TArray<FString> stringData);

	//for maintaining valid json for args in RPC calls
	static FString stringListToParsableString(TArray<FString> stringData);

	static FString int64ListToSimpleString(TArray<int64> intData);

	static FString jsonObjListToString(TArray<TSharedPtr<FJsonObject>> jsonData);
	
	static FString jsonObjListToSimpleString(TArray<TSharedPtr<FJsonObject>> jsonData);

	static FString jsonObjListToParsableString(TArray<TSharedPtr<FJsonObject>> jsonData);

	static FString jsonToString(TSharedPtr<FJsonValue> jsonData);

	static FString jsonToSimpleString(TSharedPtr<FJsonValue> jsonData);

	static FString jsonToParsableString(TSharedPtr<FJsonValue> jsonData);

	static FString jsonToParsableString(TSharedPtr<FJsonObject> jsonData);

	static FString jsonToString(TSharedPtr<FJsonObject> jsonData);

	static FString jsonToSimpleString(TSharedPtr<FJsonObject> jsonData);

	//indexer reponse extraction functions used to parse response into a frontend usable form!
	static FUpdatableHistoryArgs extractFromTransactionHistory(FString MyAddress, FGetTransactionHistoryReturn TransactionHistory);
	static FUpdatableItemDataArgs extractFromTokenBalances(FGetTokenBalancesReturn tokenBalances);

private:
	/*
	* Similar to simplifyString EXCEPT we also cleanup 
	* some special edge cases from json responses / parsing as well!
	*/
	static FString stringCleanup(FString string);

	static FMonthDayYear_BE TimestampToMonthDayYear_Be(FString Timestamp);
};
