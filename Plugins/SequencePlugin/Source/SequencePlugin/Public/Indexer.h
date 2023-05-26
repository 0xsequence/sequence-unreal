// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Struct_Data.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Http.h"
#include "Indexer.generated.h"

UENUM(BlueprintType)
enum ContractType
{
	//Unreal Disallows duplicate enumeration values within separate enums so I use a cmlCase Suffix to differentiate
    UNKOWN_CT UMETA(DisplayName = "UNKNOWN_ContractType"),
	ERC20 UMETA(DisplayName = "ERC20"),
	ERC721 UMETA(DisplayName = "ERC721"),
	ERC1155 UMETA(DisplayName = "ERC1155"),
	SEQUENCE_WALLET UMETA(DisplayName = "Sequence_Wallet"),
	ERC20_BRIDGE UMETA(DisplayName = "ERC20_Bridge"),
	ERC721_BRIDGE UMETA(DisplayName = "ERC721_Bridge"),
	ERC1155_BRIDGE UMETA(DisplayName = "ERC1155_Bridge")
};

UENUM(BlueprintType)
enum EventLogType
{
	UNKOWN_ELT UMETA(DisplayName = "UNKOWN_Event_Log_Type"),
	BLOCK_ADDED UMETA(DisplayName = "BLOCK_ADDED"),
	BLOCK_REMOVED UMETA(DisplayName = "BLOCK_REMOVED")
};

UENUM(BlueprintType)
enum EventLogDataType
{
	UNKNOWN_ELDT UMETA(DisplayName = "UNKOWN_Event_Log_Data_Type"),
	TOKEN_TRANSFER UMETA(DisplayName = "TOKEN_TRANSFER"),
	SEQUENCE_TXN UMETA(DisplayName = "SEQUENCE_TXN")
};

UENUM(BlueprintType)
enum TxnTransferType
{
	UNKNOWN_TTT UMETA(DisplayName = "UNKOWN_Transaction_Transfer_Type"),
	SEND UMETA(DisplayName = "SEND"),
	RECEIVE UMETA(DisplayName = "RECEIVE")
};

UENUM(BlueprintType)
enum SortOrder
{
	DESC UMETA(DisplayName = "Descending"),
	ASC UMETA(DisplayName = "Ascending")
};

/**
 * 
 */
UCLASS()

class SEQUENCEPLUGIN_API UIndexer : public UObject
{
	GENERATED_BODY()

private:
	//need static data here!

	//not sure what this path is supposed to go to?
	const FString PATH = "/rpc/Indexer/";

	TMap<int64, FString> Indexernames;

//private functions
	
	FString Url(int64 chainID, FString endpoint);

	FString HostName(int64 chainID);

	FString HTTPPost(int64 chainID, FString endpoint, FString args);

	template < typename T > T BuildResponse(FString text);

//end of private functions

public:

//public functions

	void testing();

	UIndexer();
	UIndexer(FString args);

	bool Ping(int64 chainID);

	int32 Version(int64 chainID);

	//need to assess task structure
	void RunTimeStatus(int64 chainID);

	int64 GetChainID(int64 chainID);//?

	void GetEtherBalance(int64 chainID, FString accountAddr);

	void GetTokenBalances(int64 chainID, FString args);

	void GetTokenSupplies(int64 chainID, FString args);

	void GetTokenSuppliesMap(int64 chainID, FString args);

	void GetBalanceUpdates(int64 chainID, FString args);

	void GetTransactionHistory(int64 chainID, FString args);

//end of public functions


};

//templated functions
/*
template<class T>
inline T UIndexer::BuildResponse(FString text)
{

	return T();
}
*/