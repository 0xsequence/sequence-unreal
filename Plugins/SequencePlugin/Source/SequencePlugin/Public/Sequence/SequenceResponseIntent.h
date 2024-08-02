// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Indexer/IndexerSupport.h"
#include "SequenceResponseIntent.generated.h"

//CloseSession//

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqCloseSessionResponse_Response
{
	GENERATED_BODY()

	UPROPERTY()
	FString Code = "";

	bool IsValid() const
	{
		return Code.Equals(TEXT("sessionClosed"),ESearchCase::IgnoreCase);
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqCloseSessionResponse
{
	GENERATED_BODY()
	UPROPERTY()
	FSeqCloseSessionResponse_Response Response;

	bool IsValid() const
	{
		return Response.IsValid();
	}
};

//CloseSession//

//SendTransaction//

USTRUCT(BlueprintType)
struct FSeqTxnLog
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
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_NativeReceipt
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
	TArray<FSeqTxnLog> Logs;
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
struct SEQUENCEPLUGIN_API FSeqTxnSimulation
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
struct SEQUENCEPLUGIN_API FSeqTxnSignature
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString SessionId = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Signature = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_Response
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
struct SEQUENCEPLUGIN_API FSeqTxnData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Identifier = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Network = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FSeqTransactionResponse_Response> Transactions;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TransactionFeeQuote = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Wallet = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_Request
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FSeqTxnData Data;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int64 ExpiresAt = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int64 IssuedAt = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Name = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FSeqTxnSignature> Signatures;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Version = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_Receipt
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Id = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int64 Index = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FSeqTxnLog> Logs;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString RevertReason = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Status = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TxnReceipt = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString IdentifyingCode = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString TxHash = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString MetaTxHash = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FSeqTransactionResponse_Request Request;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FSeqTransactionResponse_NativeReceipt NativeReceipt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FSeqTransactionResponse_Receipt Receipt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<FSeqTxnSimulation> Simulations;

	//Raw Json Response for ease of use with older code
	TSharedPtr<FJsonObject> Json;

	void Setup(const TSharedPtr<FJsonObject>& JsonIn)
	{
		Json = JsonIn;
	}
};

//SendTransaction//

//SignMessage//

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqSignMessageResponse_Data
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Message = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Signature = "";
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqSignMessageResponse_Response
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FSeqSignMessageResponse_Data Data;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqSignMessageResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FSeqSignMessageResponse_Response Response;

	bool IsValid() const
	{
		return true;
	}
};

//SignMessage//

//ListSessions//

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqListSessions_Identity
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Type = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Iss = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Sub = "";
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqListSessions_Session
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString Id = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	int64 ProjectId = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString UserId = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FSeqListSessions_Identity Identity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString FriendlyName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString CreatedAt = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString RefreshedAt = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString ExpiresAt = "";
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqListSessionResponse_Response
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FSeqListSessions_Session> Data;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqListSessionsResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FSeqListSessionResponse_Response Response;
};

//ListSessions//