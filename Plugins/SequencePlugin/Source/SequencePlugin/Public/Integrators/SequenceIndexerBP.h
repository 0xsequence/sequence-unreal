// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequenceRPCManager.h"
#include "Indexer/Indexer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceIndexerBP.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetEtherBalance, bool, Status, int64, Balance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTokenBalances, bool, Status, FSeqGetTokenBalancesReturn, Balances);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTokenSupplies, bool, Status, FSeqGetTokenSuppliesReturn, Supplies);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTokenSuppliesMap, bool, Status, FSeqGetTokenSuppliesMapReturn, SuppliesMap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTransactionHistory, bool, Status, FSeqGetTransactionHistoryReturn, TransactionHistory);

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceIndexerBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	USequenceIndexerBP();

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnGetEtherBalance EtherBalanceReceived;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnGetTokenBalances TokenBalancesReceived;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnGetTokenSupplies TokenSuppliesReceived;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnGetTokenSuppliesMap TokenSuppliesMapReceived;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnGetTransactionHistory TransactionHistoryReceived;

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SetChainId(int64 NewChainId);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetEtherBalanceAsync(const FString& WalletAddress);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetTokenBalancesAsync(const FString& WalletAddress, const FString& ContractAddress, const bool IncludeMetadata);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetTokenSuppliesAsync(const FString& ContractAddress, const bool IncludeMetadata);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetTokenSuppliesMapAsync(const FSeqGetTokenSuppliesMapArgs& Args);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetTransactionHistoryAsync(const FSeqGetTransactionHistoryArgs& Args);
	
private:
	UPROPERTY()
	UIndexer* Indexer;
	int64 ChainId = 137;
	
	void CallEtherBalanceReceived(const bool Status, const int64 Balance) const;
	void CallTokenBalancesReceived(const bool Status, const FSeqGetTokenBalancesReturn& Balances) const;
	void CallTokenSuppliesReceived(const bool Status, const FSeqGetTokenSuppliesReturn& Supplies) const;
	void CallTokenSuppliesMapReceived(const bool Status, const FSeqGetTokenSuppliesMapReturn& SuppliesMap) const;
	void CallTransactionHistoryReceived(const bool Status, const FSeqGetTransactionHistoryReturn& TransactionHistory) const;
};
