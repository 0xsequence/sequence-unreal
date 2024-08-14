// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Containers/Union.h"
#include "Types/Types.h"
#include "Util/Structs/BE_Structs.h"
#include "SequenceSupport.generated.h"

struct FGetTokenBalancesReturn;
struct FTokenBalanceExtractorReturn;
struct FGetTransactionHistoryReturn;
struct FUpdatableItemDataArgs;

typedef TPair<FString, int64> FNameId;

typedef TPair<int64, FString> FIdName;

/**
 * Support class for the indexer
 */
UCLASS()
class SEQUENCEPLUGIN_API USequenceSupport : public UObject
{
	GENERATED_BODY()

private:
	//Conversion maps for Network Id's and names
	//when indexing on names compress spaces and to lower
	
	static inline TMap<int64, FString> NetworkIdToNameMap = {
		FIdName(1,TEXT("Ethereum")),
		FIdName(10,TEXT("Optimism")),
		FIdName(56,TEXT("BNB Smart Chain")),
		FIdName(100,TEXT("Gnosis Chain")),
		FIdName(137,TEXT("Polygon")),
		FIdName(1101,TEXT("Polygon zkEVM")),
		FIdName(3776,TEXT("Astar zkEVM")),
		FIdName(8453,TEXT("Base (Coinbase)")),
		FIdName(19011,TEXT("Oasys Homeverse")),
		FIdName(42161,TEXT("Arbitrum One")),
		FIdName(42170,TEXT("Arbitrum Nova")),
		FIdName(43114,TEXT("Avalanche")),
		FIdName(81457,TEXT("Blast")),
		FIdName(660279,TEXT("Xai")),
		FIdName(97,TEXT("BNB Smart Chain Testnet")),
		FIdName(1993,TEXT("B3 Sepolia")),
		FIdName(2730,TEXT("XR Sepolia")),
		FIdName(33111,TEXT("APE Chain Testnet")),
		FIdName(40875,TEXT("Oasys Homeverse Testnet")),
		FIdName(43113,TEXT("Avalanche Testnet")),
		FIdName(80002,TEXT("Polygon Amoy")),
		FIdName(84532,TEXT("Base Sepolia")),
		FIdName(421614,TEXT("Arbitrum Sepolia")),
		FIdName(6038361,TEXT("Astar zKyoto Testnet")),
		FIdName(11155111,TEXT("Sepolia")),
		FIdName(11155420,TEXT("Optimism Sepolia")),
		FIdName(168587773,TEXT("Blast Sepolia")),
		FIdName(37714555429,TEXT("Xai Sepolia"))};

	static inline TMap<FString, int64> NetworkNameToIdMap = {
		FNameId(TEXT("ethereum"),1),
		FNameId(TEXT("optimism"),10),
		FNameId(TEXT("bnbsmartchain"),56),
		FNameId(TEXT("gnosischain"),100),
		FNameId(TEXT("polygon"),137),
		FNameId(TEXT("polygonzkevm"),1101),
		FNameId(TEXT("astarzkevm"),3776),
		FNameId(TEXT("base(coinbase)"),8453),
		FNameId(TEXT("base"),8453),
		FNameId(TEXT("oasyshomeverse"),19011),
		FNameId(TEXT("arbitrumone"),42161),
		FNameId(TEXT("arbitrumnova"),42170),
		FNameId(TEXT("avalanche"),43114),
		FNameId(TEXT("blast"),81457),
		FNameId(TEXT("xai"),660279),
		FNameId(TEXT("bnbsmartchaintestnet"),97),
		FNameId(TEXT("b3sepolia"),1993),
		FNameId(TEXT("xrsepolia"),2730),
		FNameId(TEXT("apechaintestnet"),33111),
		FNameId(TEXT("oasyshomeversetestnet"),40875),
		FNameId(TEXT("avalanchetestnet"),43113),
		FNameId(TEXT("polygonamoy"),80002),
		FNameId(TEXT("basesepolia"),84532),
		FNameId(TEXT("arbitrumsepolia"),421614),
		FNameId(TEXT("astarzkyototestnet"),6038361),
		FNameId(TEXT("sepolia"),11155111),
		FNameId(TEXT("optimismsepolia"),11155420),
		FNameId(TEXT("blastsepolia"),168587773),
		FNameId(TEXT("xaisepolia"),37714555429)};
	
public:
	
	/**
	 * Pass in a network Id and receive the name associated with that Id
	 * @param NetworkIdIn The Id associated with the Network Name
	 * @return The matching network name, OR an empty FString if nothing was found
	 */
	static FString GetNetworkName(const int64 NetworkIdIn);

	/**
	 * Gets the associated Network Id with the given Network Name
	 * @param NetworkNameIn The network's name
	 * @return The associated Id, if none was found returns -1
	 */
	static int64 GetNetworkId(const FString& NetworkNameIn);

	/**
	 * Gets all networks supported by sequence
	 * @return A list of Pairs of Network Ids & Names
	 */
	static TArray<FIdName> GetAllNetworks();

	/**
	 * Gets all Network Names supported by sequence
	 * @return A list of all network names in the system
	 */
	static TArray<FString> GetAllNetworkNames();

	/**
	 * Gets a list of all network Ids support by sequence
	 * @return A list of all network ids in the system
	 */
	static TArray<int64> GetAllNetworkIds();
	
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
