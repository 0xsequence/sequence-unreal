// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Dom/JsonObject.h"
#include "TransactionResponse.generated.h"

USTRUCT(BlueprintType)
struct FLog
{
	GENERATED_BODY()

	FString Address = "";
	FString BlockHash = "";
	FString BlockNumber = "";
	FString Data = "";
	FString LogIndex = "";
	bool Removed = false;
	TArray<FString> Topics;
	FString TransactionHash = "";
	FString TransactionIndex = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FNativeReceipt
{
	GENERATED_BODY()

	FString BlockHash = "";
	FString BlockNumber = "";
	FString ContractAddress = "";
	FString CumulativeGasUsed = "";
	FString GasUsed = "";
	TArray<FLog> Logs;
	FString LogsBloom = "";
	FString Root = "";
	FString Status = "";
	FString TransactionHash = "";
	FString TransactionIndex = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSimulation
{
	GENERATED_BODY()

	bool Executed = false;
	int64 GasLimit = 0;
	int64 GasUsed = 0;
	FString Reason = "";
	FString Result = "";
	bool Succeeded = false;
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSignature
{
	GENERATED_BODY()

	FString SessionId = "";
	FString Signature = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTransactionResponse//find a good name for these structures as there may be a large amount of overlap in naming
{//OR verify if the type already exists as it very well might!
	FString To = "";
	FString TokenAddress = "";
	FString Type = "";//Look for existing enums for this!
	FString Value = "";	
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FData
{
	GENERATED_BODY()

	FString Identifier = "";
	FString Network = "";
	//transactions//
	FString TransactionFeeQuote = "";
	FString Wallet = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FRequest
{
	GENERATED_BODY()

	FData Data;
	int64 ExpiresAt = 0;
	int64 IssuedAt = 0;
	FString Name = "";
	TArray<FSignature> Signatures;
	FString Version = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FReceiptEntry
{
	GENERATED_BODY()

	FString Id = "";
	int64 Index = 0;
	TArray<FLog> Logs;
	//Receipts are recursive in definition which is fine as FJson structures but not as UStruct
	//this nesting will make it impossible to bring into USTRUCTS, sleep on this because I may need to slice off where I can handle / worth doing
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FReceipt
{
	GENERATED_BODY()

	FString Id = "";
	int64 Index = 0;
	TArray<FLog> Logs;
	//TArray<FReceipt> Receipts
	FString RevertReason = "";
	FString Status = "";
	FString TxnReceipt = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTransactionResponse
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString IdentifyingCode = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TxHash = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MetaTxHash = "";

	//these 4 portions are going to get processed into UStructs for ease of use in blueprints
	
	TSharedPtr<FJsonObject> Request;
	TSharedPtr<FJsonObject> NativeReceipt;//Done
	TSharedPtr<FJsonObject> Receipt;
	TArray<TSharedPtr<FJsonValue>> Simulations;

	//Raw Json Response for ease of use with older code
	TSharedPtr<FJsonObject> Json;

	FTransactionResponse(){}
	
	FTransactionResponse(
		const FString& IdentifyingCodeIn, const FString& TxHashIn, const FString& MetaTxHashIn,
		const TSharedPtr<FJsonObject>& RequestIn,
		const TSharedPtr<FJsonObject>& NativeReceiptIn,
		const TSharedPtr<FJsonObject>& ReceiptIn,
		const TArray<TSharedPtr<FJsonValue>>& SimulationsIn,
		const TSharedPtr<FJsonObject>& JsonIn)
	{
		IdentifyingCode = IdentifyingCodeIn;
		TxHash = TxHashIn;
		MetaTxHash = MetaTxHashIn;
		Request = RequestIn;
		NativeReceipt = NativeReceiptIn;
		Simulations = SimulationsIn;
		Json = JsonIn;
	}
};