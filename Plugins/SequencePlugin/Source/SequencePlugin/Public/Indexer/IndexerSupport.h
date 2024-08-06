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
	
	/*
	* Used to get the amount that users can read,
	* int64 Amount, the integer amount of Crypto
	* int64 Decimals, the integer amount of Decimal places used to represent the Amount
	* Return, The user readable Float amount
	*/
	static float GetAmount(const int64 Amount, const int64 Decimals);
	
	/*
	* Used to convert a user readable amount into a usable amount value for transactions,
	* float Amount, the user readable amount they wish to use
	* int64 Decimals, the integer amount of Decimal places used to represent the Amount
	* Return, The transaction usable amount
	*/
	static int64 GetAmount(const float Amount, const int64 Decimals);

	static FString GetStringFromToken(const FString& IdToken, const FString& ParameterName);

	static int32 GetInt32FromToken(const FString& IdToken, const FString& ParameterName);
	
	static int64 GetInt64FromToken(const FString& IdToken, const FString& ParameterName);

	static float GetFloatFromToken(const FString& IdToken, const FString& ParameterName);

	static double GetDoubleFromToken(const FString& IdToken, const FString& ParameterName);

	static bool GetBoolFromToken(const FString& IdToken, const FString& ParameterName);
	
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

	/**
	 * Converts a UStruct into a Json usable String
	 * @tparam T Templated UStruct
	 * @param StructVar Struct to parse
	 * @return JsonString version of parsed UStruct
	 */
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

	static int64 StringDateToUnixDate(const FString& Iso8601);
	
private:
	/*
	* Similar to simplifyString EXCEPT we also cleanup 
	* some special edge cases from json responses / parsing as well!
	*/
	static FString StringCleanup(FString String);

	static FMonthDayYear_BE TimestampToMonthDayYear_Be(FString Timestamp);
};
