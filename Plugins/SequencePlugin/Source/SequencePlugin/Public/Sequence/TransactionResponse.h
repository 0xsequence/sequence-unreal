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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Address = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString BlockHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString BlockNumber = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Data = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString LogIndex = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Removed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FString> Topics;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString TransactionHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString TransactionIndex = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnNativeReceipt
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString BlockHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString BlockNumber = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ContractAddress = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString CumulativeGasUsed = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString GasUsed = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTxnLog> Logs;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString LogsBloom = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Root = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Status = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString TransactionHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString TransactionIndex = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnSimulation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Executed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int64 GasLimit = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int64 GasUsed = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Reason = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Result = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Succeeded = false;
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnSignature
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString SessionId = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Signature = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnTransactionResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString To = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString TokenAddress = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Type = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Value = "";	
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Identifier = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Network = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTxnTransactionResponse> Transactions;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString TransactionFeeQuote = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Wallet = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTxnData Data;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int64 ExpiresAt = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int64 IssuedAt = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTxnSignature> Signatures;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Version = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTxnReceipt
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Id = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int64 Index = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTxnLog> Logs;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString RevertReason = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Status = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTxnRequest Request;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTxnNativeReceipt NativeReceipt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTxnReceipt Receipt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTxnSimulation> Simulations;

	//Raw Json Response for ease of use with older code
	TSharedPtr<FJsonObject> Json;

	void Setup(const TSharedPtr<FJsonObject>& JsonIn)
	{
		Json = JsonIn;
	}
};