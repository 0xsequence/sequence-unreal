#pragma once

#include "CoreMinimal.h"
#include "Indexer_Enums.generated.h"

UENUM(BlueprintType)
enum EContractType
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
enum EEventLogType
{
	UNKOWN_ELT UMETA(DisplayName = "UNKOWN_Event_Log_Type"),
	BLOCK_ADDED UMETA(DisplayName = "BLOCK_ADDED"),
	BLOCK_REMOVED UMETA(DisplayName = "BLOCK_REMOVED")
};

UENUM(BlueprintType)
enum EEventLogDataType
{
	UNKNOWN_ELDT UMETA(DisplayName = "UNKOWN_Event_Log_Data_Type"),
	TOKEN_TRANSFER UMETA(DisplayName = "TOKEN_TRANSFER"),
	SEQUENCE_TXN UMETA(DisplayName = "SEQUENCE_TXN")
};

UENUM(BlueprintType)
enum ETxnTransferType
{
	UNKNOWN_TTT UMETA(DisplayName = "UNKOWN_Transaction_Transfer_Type"),
	SEND UMETA(DisplayName = "SEND"),
	RECEIVE UMETA(DisplayName = "RECEIVE")
};

UENUM(BlueprintType)
enum ESortOrder_Sequence
{
	DESC UMETA(DisplayName = "Descending"),
	ASC UMETA(DisplayName = "Ascending")
};