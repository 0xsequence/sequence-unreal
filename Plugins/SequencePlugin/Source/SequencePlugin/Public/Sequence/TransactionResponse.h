// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Dom/JsonObject.h"
#include "TransactionResponse.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FTransactionResponse
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString IdentifyingCode = "";
	UPROPERTY()
	FString TxHash = "";
	UPROPERTY()
	FString MetaTxHash = "";

	TSharedPtr<FJsonObject> Request;
	TSharedPtr<FJsonObject> NativeReceipt;
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