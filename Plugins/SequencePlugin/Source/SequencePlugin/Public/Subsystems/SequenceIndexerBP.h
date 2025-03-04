// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequenceRPCManager.h"
#include "Indexer/Indexer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceIndexerBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceIndexerBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetEtherBalance, const FString&, WalletAddress, int64, Balance);
	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnGetTokenBalances, const FString&, WalletAddress, const FString&, ContractAddress, FSeqGetTokenBalancesReturn, Balances);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetTokenSupplies, const FString&, ContractAddress, FSeqGetTokenSuppliesReturn, Supplies);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetTokenSuppliesMap, const FSeqGetTokenSuppliesMapArgs&, Request, FSeqGetTokenSuppliesMapReturn, SuppliesMap);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetTransactionHistory, const FSeqGetTransactionHistoryArgs&, Request, FSeqGetTransactionHistoryReturn, TransactionHistory);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	
public:
	USequenceIndexerBP();
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SetChainById(const int64 NewChainId);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SetChainByName(const FString& NewChainName);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SetChainByType(const ENetwork& NewChainType);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetEtherBalanceAsync(const FString& WalletAddress, FOnGetEtherBalance OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetTokenBalancesAsync(const FString& WalletAddress, const FString& ContractAddress, const bool IncludeMetadata, FOnGetTokenBalances OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetTokenSuppliesAsync(const FString& ContractAddress, const bool IncludeMetadata, FOnGetTokenSupplies OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetTokenSuppliesMapAsync(const FSeqGetTokenSuppliesMapArgs& Args, FOnGetTokenSuppliesMap OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetTransactionHistoryAsync(const FSeqGetTransactionHistoryArgs& Args, FOnGetTransactionHistory OnSuccess, FOnFailure OnFailure);
	
private:
	UPROPERTY()
	UIndexer* Indexer;
	int64 ChainId = 137;
};
