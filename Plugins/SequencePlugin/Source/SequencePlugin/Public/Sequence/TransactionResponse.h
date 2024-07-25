// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Dom/JsonObject.h"
#include "TransactionResponse.generated.h"

USTRUCT(BlueprintType)
struct FTxnLog
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Address = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString BlockHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString BlockNumber = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Data = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString LogIndex = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool Removed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FString> Topics;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TransactionHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TransactionIndex = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnNativeReceipt
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString BlockHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString BlockNumber = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString ContractAddress = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString CumulativeGasUsed = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString GasUsed = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FTxnLog> Logs;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString LogsBloom = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Root = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Status = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TransactionHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TransactionIndex = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnSimulation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool Executed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int64 GasLimit = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int64 GasUsed = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Reason = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Result = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool Succeeded = false;
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnSignature
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString SessionId = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Signature = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnTransactionResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString To = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TokenAddress = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Type = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Value = "";	
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Identifier = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Network = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FTxnTransactionResponse> Transactions;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TransactionFeeQuote = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Wallet = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FTxnData Data;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int64 ExpiresAt = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int64 IssuedAt = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Name = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FTxnSignature> Signatures;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Version = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnReceipt
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Id = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int64 Index = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FTxnLog> Logs;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString RevertReason = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Status = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TxnReceipt = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTransactionResponse
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString IdentifyingCode = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString TxHash = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString MetaTxHash = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FTxnRequest Request;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FTxnNativeReceipt NativeReceipt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FTxnReceipt Receipt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<FTxnSimulation> Simulations;

	//Raw Json Response for ease of use with older code
	TSharedPtr<FJsonObject> Json;

	void Setup(const TSharedPtr<FJsonObject>& JsonIn)
	{
		Json = JsonIn;
	}
};