// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Containers/Union.h"
#include "Types/Types.h"
#include "Util/Structs/BE_Structs.h"
#include "Util/Structs/BE_Enums.h"
#include "SequenceSupport.generated.h"

struct FSeqGetTokenBalancesReturn;
struct FTokenBalanceExtractorReturn;
struct FSeqGetTransactionHistoryReturn;
struct FUpdatableItemDataArgs;

typedef TPair<FString, int64> FNameId;
typedef TPair<int64, FString> FIdName;

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FIdNamePair
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Network Data")
	int64 NetworkId = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Network Data")
	FString NetworkName = "";

	FIdNamePair(){}

	FIdNamePair(const FIdName& IdNameIn)
	{
		NetworkId = IdNameIn.Key;
		NetworkName = IdNameIn.Value;
	}
};

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

	static inline TMap<int64, FString> NetworkIdToUrlMap = {
		FIdName(1,TEXT("mainnet")),
		FIdName(10,TEXT("optimism")),
		FIdName(40,TEXT("telos")),
		FIdName(56,TEXT("bsc")),
		FIdName(100,TEXT("gnosis")),
		FIdName(137,TEXT("polygon")),
		FIdName(273,TEXT("xr1")),
		FIdName(1101,TEXT("polygon-zkevm")),
		FIdName(1284,TEXT("moonbeam")),
		FIdName(1868,TEXT("soneium")),
		FIdName(6283,TEXT("laos")),
		FIdName(7668,TEXT("rootnet")),
		FIdName(8333,TEXT("b3")),
		FIdName(8453,TEXT("base")),
		FIdName(13371,TEXT("immutable-zkevm")),
		FIdName(19011,TEXT("homeverse")),
		FIdName(33139,TEXT("apechain")),
		FIdName(42161,TEXT("arbitrum")),
		FIdName(42170,TEXT("arbitrum-nova")),
		FIdName(42793,TEXT("etherlink")),
		FIdName(43114,TEXT("avalanche")),
		FIdName(81457,TEXT("blast")),
		FIdName(660279,TEXT("xai")),
		FIdName(1482601649,TEXT("skale-nebula")),
		
		FIdName(41,TEXT("telos-testnet")),
		FIdName(97,TEXT("bsc-testnet")),
		FIdName(1287,TEXT("moonbase-alpha")),
		FIdName(1946,TEXT("soneium-minato")),
		FIdName(1993,TEXT("b3-sepolia")),
		FIdName(2730,TEXT("xr-sepolia")),
		FIdName(7672,TEXT("rootnet-porcini")),
		FIdName(10143,TEXT("monad-testnet")),
		FIdName(13473,TEXT("immutable-zkevm-testnet")),
		FIdName(33111,TEXT("apechain-testnet")),
		FIdName(40875,TEXT("homeverse-testnet")),
		FIdName(43113,TEXT("avalanche-testnet")),
		FIdName(50312,TEXT("somnia-testnet")),
		FIdName(62850,TEXT("laos-sigma-testnet")),
		FIdName(80002,TEXT("amoy")),
		FIdName(84532,TEXT("base-sepolia")),
		FIdName(128123,TEXT("etherlink-testnet")),
		FIdName(421614,TEXT("arbitrum-sepolia")),
		FIdName(11155111,TEXT("sepolia")),
		FIdName(11155420,TEXT("optimism-sepolia")),
		FIdName(21000000,TEXT("toy-testnet")),
		FIdName(37084624,TEXT("skale-nebula-testnet")),
		FIdName(168587773,TEXT("blast-sepolia")),
		FIdName(37714555429,TEXT("xai-sepolia"))};
	
	static inline TMap<int64, FString> NetworkIdToNameMap = {
		FIdName(1,TEXT("Ethereum")),
		FIdName(13371,TEXT("Immutable")),
		FIdName(1482601649,TEXT("SKALE Nebula")),
		FIdName(10,TEXT("Optimism")),
		FIdName(56,TEXT("BNB Smart Chain")),
		FIdName(100,TEXT("Gnosis Chain")),
		FIdName(137,TEXT("Polygon")),
		FIdName(1101,TEXT("Polygon zkEVM")),
		FIdName(8333,TEXT("B3")),
		FIdName(8453,TEXT("Base (Coinbase)")),
		FIdName(19011,TEXT("Oasys Homeverse")),
		FIdName(33139,TEXT("APE Chain")),
		FIdName(42161,TEXT("Arbitrum One")),
		FIdName(42170,TEXT("Arbitrum Nova")),
		FIdName(43114,TEXT("Avalanche")),
		FIdName(81457,TEXT("Blast")),
		FIdName(660279,TEXT("Xai")),
		FIdName(6283,TEXT("LAOS")),
		FIdName(7668,TEXT("Root Network")),
		FIdName(1868,TEXT("Soneium")),
		FIdName(40,TEXT("Telos")),
		FIdName(1284,TEXT("Moonbeam")),
		FIdName(42793, TEXT("Etherlink")),
		FIdName(273, TEXT("XR1")),
		
		FIdName(13473,TEXT("Immutable Testnet")),
		FIdName(97,TEXT("BNB Smart Chain Testnet")),
		FIdName(1946,TEXT("Soneium Minato Testnet")),
		FIdName(1993,TEXT("B3 Sepolia")),
		FIdName(2730,TEXT("XR Sepolia")),
		FIdName(33111,TEXT("APE Chain Testnet")),
		FIdName(40875,TEXT("Oasys Homeverse Testnet")),
		FIdName(43113,TEXT("Avalanche Testnet")),
		FIdName(80002,TEXT("Polygon Amoy")),
		FIdName(84532,TEXT("Base Sepolia")),
		FIdName(421614,TEXT("Arbitrum Sepolia")),
		FIdName(11155111,TEXT("Sepolia")),
		FIdName(11155420,TEXT("Optimism Sepolia")),
		FIdName(21000000,TEXT("TOY Testnet")),
		FIdName(37084624,TEXT("SKALE Nebula Gaming Hub Testnet")),
		FIdName(168587773,TEXT("Blast Sepolia")),
		FIdName(37714555429,TEXT("Xai Sepolia")),
		FIdName(62850,TEXT("LAOS Sigma Testnet")),
		FIdName(7672,TEXT("Root Network Porcini Testnet")),
		FIdName(41,TEXT("Telos Testnet")),
		FIdName(1287,TEXT("Moonbase Alpha")),
		FIdName(128123,TEXT("Etherlink Testnet")),
		FIdName(10143,TEXT("Monad Testnet")),
		FIdName(50312,TEXT("Somnia Testnet")),
	};

	static inline TMap<FString, int64> NetworkNameToIdMap = {
		FNameId(TEXT("ethereum"),1),
		FNameId(TEXT("immutable"),13371),
		FNameId(TEXT("skale-nebula"),1482601649),
		FNameId(TEXT("optimism"),10),
		FNameId(TEXT("bnbsmartchain"),56),
		FNameId(TEXT("gnosischain"),100),
		FNameId(TEXT("polygon"),137),
		FNameId(TEXT("polygonzkevm"),1101),
		FNameId(TEXT("b3"),8333),
		FNameId(TEXT("base"),8453),
		FNameId(TEXT("oasyshomeverse"),19011),
		FNameId(TEXT("apechain"),33139),
		FNameId(TEXT("arbitrumone"),42161),
		FNameId(TEXT("arbitrumnova"),42170),
		FNameId(TEXT("avalanche"),43114),
		FNameId(TEXT("blast"),81457),
		FNameId(TEXT("xai"),660279),
		FNameId(TEXT("laos"),6283),
		FNameId(TEXT("rootnet"),7668),
		FNameId(TEXT("soneium"),1868),
		FNameId(TEXT("telos"),40),
		FNameId(TEXT("moonbeam"),1284),
		FNameId(TEXT("etherlink"),42793),
		FNameId(TEXT("xr1"),273),
		
		FNameId(TEXT("immutable-testnet"),13473),
		FNameId(TEXT("bnbsmartchaintestnet"),97),
		FNameId(TEXT("soneiumminato"),1946),
		FNameId(TEXT("b3sepolia"),1993),
		FNameId(TEXT("xrsepolia"),2730),
		FNameId(TEXT("apechaintestnet"),33111),
		FNameId(TEXT("oasyshomeversetestnet"),40875),
		FNameId(TEXT("avalanche-testnet"),43113),
		FNameId(TEXT("polygonamoy"),80002),
		FNameId(TEXT("basesepolia"),84532),
		FNameId(TEXT("arbitrumsepolia"),421614),
		FNameId(TEXT("sepolia"),11155111),
		FNameId(TEXT("optimismsepolia"),11155420),
		FNameId(TEXT("toytestnet"),21000000),
		FNameId(TEXT("skalenebulagaminghubtestnet"),37084624),
		FNameId(TEXT("blastsepolia"),168587773),
		FNameId(TEXT("xaisepolia"),37714555429),
		FNameId(TEXT("laos-sigma"),62850),
		FNameId(TEXT("rootnet-porcini"),7672),
		FNameId(TEXT("telos-testnet"), 41),
		FNameId(TEXT("moonbase-alpha"),1287),
		FNameId(TEXT("etherlink-testnet"),128123),
		FNameId(TEXT("monad-testnet"),10143),
		FNameId(TEXT("somnia-testnet"),50312),
	};
	
	static inline TMap<ENetwork, int64> NetworkEnumToIdMap = {
		{ENetwork::Ethereum, 1},
		{ENetwork::Immutable, 13371},
		{ENetwork::SkaleNebula, 1482601649},
		{ENetwork::Optimism, 10},
		{ENetwork::BNBSmartChain, 56},
		{ENetwork::GnosisChain, 100},
		{ENetwork::PolygonChain, 137},
		{ENetwork::PolygonZkEVM, 1101},
		{ENetwork::B3, 8333},
		{ENetwork::Base, 8453},
		{ENetwork::OasysHomeverse, 19011},
		{ENetwork::APEChain, 33139},
		{ENetwork::ArbitrumOne, 42161},
		{ENetwork::ArbitrumNova, 42170},
		{ENetwork::Avalanche, 43114},
		{ENetwork::Blast, 81457},
		{ENetwork::Xai, 660279},
		{ENetwork::LAOS, 6283},
		{ENetwork::Root, 7668},
		{ENetwork::Soneium, 1868},
		{ENetwork::Telos, 40},
		{ENetwork::Moonbeam, 1284},
		{ENetwork::Etherlink, 42793},
		{ENetwork::XR1, 273},
		
		{ENetwork::ImmutableTestnet, 13473},
		{ENetwork::BNBSmartChainTestnet, 97},
		{ENetwork::SoneiumMinatoTestnet, 1946},
		{ENetwork::B3Sepolia, 1993},
		{ENetwork::XRSepolia, 2730},
		{ENetwork::APEChainTestnet, 33111},
		{ENetwork::OasysHomeverseTestnet, 40875},
		{ENetwork::AvalancheTestnet, 43113},
		{ENetwork::PolygonAmoy, 80002},
		{ENetwork::BaseSepolia, 84532},
		{ENetwork::ArbitrumSepolia, 421614},
		{ENetwork::Sepolia, 11155111},
		{ENetwork::OptimismSepolia, 11155420},
		{ENetwork::TOYTestnet, 21000000},
		{ENetwork::SKALENebulaGamingHubTestnet, 37084624},
		{ENetwork::BlastSepolia, 168587773},
		{ENetwork::XaiSepolia, 37714555429},
		{ENetwork::LAOSSigma, 62850},
		{ENetwork::RootPorcini, 7672},
		{ENetwork::TelosTestnet, 41},
		{ENetwork::MoonbaseAlpha, 1287},
		{ENetwork::EtherlinkTestnet, 128123},
		{ENetwork::MonadTestnet, 10143},
		{ENetwork::SomniaTestnet, 50312},
	};

	static inline TMap<ENetwork, FString> NetworkEnumToNameMap = {
		{ENetwork::Ethereum, TEXT("Ethereum")},
		{ENetwork::Immutable, TEXT("Immutable")},
		{ENetwork::SkaleNebula, TEXT("SKALE Nebula")},
		{ENetwork::Optimism, TEXT("Optimism")},
		{ENetwork::BNBSmartChain, TEXT("BNB Smart Chain")},
		{ENetwork::GnosisChain, TEXT("Gnosis Chain")},
		{ENetwork::PolygonChain, TEXT("Polygon")},
		{ENetwork::PolygonZkEVM, TEXT("Polygon zkEVM")},
		{ENetwork::B3, TEXT("B3")},
		{ENetwork::Base, TEXT("Base (Coinbase)")},
		{ENetwork::OasysHomeverse, TEXT("Oasys Homeverse")},
		{ENetwork::APEChain, TEXT("APE Chain")},
		{ENetwork::ArbitrumOne, TEXT("Arbitrum One")},
		{ENetwork::ArbitrumNova, TEXT("Arbitrum Nova")},
		{ENetwork::Avalanche, TEXT("Avalanche")},
		{ENetwork::Blast, TEXT("Blast")},
		{ENetwork::Xai, TEXT("Xai")},
		{ENetwork::LAOS, TEXT("LAOS")},
		{ENetwork::Root, TEXT("Root Network")},
		{ENetwork::Soneium, TEXT("Soneium")},
		{ENetwork::Telos, TEXT("Telos")},
		{ENetwork::Moonbeam, TEXT("Moonbeam")},
		{ENetwork::Etherlink, TEXT("Etherlink")},
		{ENetwork::XR1, TEXT("XR1")},
		
		{ENetwork::ImmutableTestnet, TEXT("Immutable Testnet")},
		{ENetwork::BNBSmartChainTestnet, TEXT("BNB Smart Chain Testnet")},
		{ENetwork::SoneiumMinatoTestnet, TEXT("Soneium Minato Testnet")},
		{ENetwork::B3Sepolia, TEXT("B3 Sepolia")},
		{ENetwork::XRSepolia, TEXT("XR Sepolia")},
		{ENetwork::APEChainTestnet, TEXT("APE Chain Testnet")},
		{ENetwork::OasysHomeverseTestnet, TEXT("Oasys Homeverse Testnet")},
		{ENetwork::AvalancheTestnet, TEXT("Avalanche Testnet")},
		{ENetwork::PolygonAmoy, TEXT("Polygon Amoy")},
		{ENetwork::BaseSepolia, TEXT("Base Sepolia")},
		{ENetwork::ArbitrumSepolia, TEXT("Arbitrum Sepolia")},
		{ENetwork::Sepolia, TEXT("Sepolia")},
		{ENetwork::OptimismSepolia, TEXT("Optimism Sepolia")},
		{ENetwork::TOYTestnet, TEXT("TOY Testnet")},
		{ENetwork::SKALENebulaGamingHubTestnet, TEXT("SKALE Nebula Gaming Hub Testnet")},
		{ENetwork::BlastSepolia, TEXT("Blast Sepolia")},
		{ENetwork::XaiSepolia, TEXT("Xai Sepolia")},
		{ENetwork::LAOSSigma, TEXT("LAOS Sigma Testnet")},
		{ENetwork::RootPorcini, TEXT("Root Network Porcini Testnet")},
		{ENetwork::TelosTestnet, TEXT("Telos Testnet")},
		{ENetwork::MoonbaseAlpha, TEXT("Moonbase Alpha")},
		{ENetwork::EtherlinkTestnet, TEXT("Etherlink Testnet")},
		{ENetwork::MonadTestnet, TEXT("Monad Testnet")},
		{ENetwork::SomniaTestnet, TEXT("Somnia Testnet")},
	};

public:
	
	/**
	 * Pass in a network Id and receive the name associated with that Id
	 * @param NetworkIdIn The Id associated with the Network Name
	 * @return The matching network name, OR an empty FString if nothing was found
	 */
	static FString GetNetworkName(const int64 NetworkIdIn);

	static FString GetNetworkName(const ENetwork NetworkIn);


	static FString GetNetworkNameForUrl(const int64 NetworkIdIn);

	static bool IsNetworkIdSupported(const int64 NetworkIdIn);

	/**
	 * Gets the associated Network Id with the given Network Name
	 * @param NetworkNameIn The network's name
	 * @return The associated Id, if none was found returns -1
	 */
	static int64 GetNetworkId(const FString& NetworkNameIn);

	static int64 GetNetworkId(const ENetwork& Network);

	/**
	 * Gets all networks in the system
	 * @return a TArray of all networks
	 */
	static TArray<FIdNamePair> GetAllNetworks();

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
	static float GetUserReadableAmount(const int64 Amount, const int64 Decimals);
	
	/*
	* Used to convert a user readable amount into a usable amount value for transactions,
	* float Amount, the user readable amount they wish to use
	* int64 Decimals, the integer amount of Decimal places used to represent the Amount
	* Return, The transaction usable amount
	*/
	static int64 GetSystemReadableAmount(const float Amount, const int64 Decimals);

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
	static TMap<FString, FString> JSONObjectParser(const TSharedPtr<FJsonObject>& JSONData);

	static TSharedPtr<FJsonObject> JsonStringToObject(const FString& JSON);
	
	/*
	* Used to remove
	* \n, \", \t, \r, spaces etc
	*/
	static FString SimplifyString(FString String);

	static FString PartialSimpleString(FString String);

	/*
	* Similar to simplify string EXCEPT we keep the /" because we are trying
	* to maintain json correctness for parsing!
	*/
	static FString SimplifyStringParsable(FString String);

	/*
	* Allows for the converting of a Struct straight into a nicely formatted json string!
	*/
	template < typename T > static FString StructToString(T StructVar)
	{
		FString Ret;
		FJsonObjectConverter::UStructToJsonObjectString<T>(StructVar, Ret, 0, 0);
		StringReplace(&Ret, "\n", "");
		return Ret;
	}

	/*
	* Allows for the converting of a Struct straight into a nicely formatted json string!
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

	static TArray<TSharedPtr<FJsonValue>> StringListToJsonArray(const TArray<FString>& StringData);

	static FString Int64ListToSimpleString(TArray<int64> IntData);

	static FString JsonObjListToString(TArray<TSharedPtr<FJsonObject>> JsonData);
	
	static FString JsonObjListToSimpleString(TArray<TSharedPtr<FJsonObject>> JsonData);

	static FString JsonObjListToParsableString(TArray<TSharedPtr<FJsonObject>> JsonData);

	static FString JsonValueListToParsableString(const TArray<TSharedPtr<FJsonValue>>& JsonData);

	static FString JsonToString(const TSharedPtr<FJsonValue>& JsonData);

	static FString JsonToSimpleString(const TSharedPtr<FJsonValue>& JsonData);

	static FString JsonToParsableString(const TSharedPtr<FJsonValue>& JsonData);

	static FString JsonToParsableString(const TSharedPtr<FJsonObject>& JsonData);

	static FString JsonToString(const TSharedPtr<FJsonObject>& JsonData);

	static FString JsonToSimpleString(const TSharedPtr<FJsonObject>& JsonData);

	static int64 StringDateToUnixDate(const FString& Iso8601);
	
private:
	/*
	* Similar to simplifyString EXCEPT we also cleanup 
	* some special edge cases from json responses / parsing as well!
	*/
	static FString StringCleanup(FString String);
	static void StringReplace(FString* Input, const FString& Search, const FString& Replacement);
};
