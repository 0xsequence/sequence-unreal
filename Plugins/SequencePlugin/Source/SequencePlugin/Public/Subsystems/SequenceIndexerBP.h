// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequenceRPCManager.h"
#include "Indexer/SequenceIndexer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceIndexerBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceIndexerBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetNativeTokenBalance, int64, Balance);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetTokenBalances, const TArray<FSeqTokenBalance>&, Balances, FSeqPage, Page);
	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnGetTokenSupplies, const TArray<FSeqTokenSupply>&, Tokens, EContractType, ContractType, FSeqPage, Page);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetTokenSuppliesMap, FSeqGetTokenSuppliesMapReturn, Return);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetTransactionHistory, const TArray<FSeqTransaction>&, Transactions, FSeqPage, Page);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRuntimeStatus, FSeqRuntimeStatus, Status);
	DECLARE_DYNAMIC_DELEGATE_FourParams(FOnVersion, FString, WebRpcVersion, FString, SchemaVersion, FString, SchemaHash, FString, AppVersion);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPing, bool, Success);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	
public:
	USequenceIndexerBP();
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Indexer")
	void GetNativeTokenBalance(const FString& WalletAddress, FOnGetNativeTokenBalance OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetTokenBalances(const FString& WalletAddress, const FString& ContractAddress, const bool IncludeMetadata, FOnGetTokenBalances OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetTokenSupplies(const FString& ContractAddress, const bool IncludeMetadata, FOnGetTokenSupplies OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetTokenSuppliesMap(const TMap<FString, FSeqTokenList>& TokenMap, const bool IncludeMetadata, FOnGetTokenSuppliesMap OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetTransactionHistory(const FSeqTransactionHistoryFilter& Filter, const FSeqPage& Page, const bool IncludeMetadata, FOnGetTransactionHistory OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void RuntimeStatus(FOnRuntimeStatus OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void Version(FOnVersion OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void Ping(FOnPing OnSuccess, FOnFailure OnFailure);
	
private:
	UPROPERTY()
	USequenceIndexer* Indexer;
};
