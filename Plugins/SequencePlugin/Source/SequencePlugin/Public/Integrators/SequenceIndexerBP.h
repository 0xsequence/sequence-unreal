// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequenceRPCManager.h"
#include "Indexer/Indexer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Indexer/Structs/SeqGetTokenBalancesArgs.h"
#include "SequenceIndexerBP.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetEtherBalance, bool, Status, int64, Balance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTokenBalances, bool, Status, FSeqGetTokenBalancesReturn, Balances);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTokenSupplies, bool, Status, FSeqGetTokenSuppliesReturn, Supplies);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTokenSuppliesMap, bool, Status, FSeqGetTokenSuppliesMapReturn, SuppliesMap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetBalanceUpdates, bool, Status, FSeqGetBalanceUpdatesReturn, BalanceUpdates);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTransactionHistory, bool, Status, FSeqGetTransactionHistoryReturn, TransactionHistory);

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceIndexerBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	USequenceIndexerBP();

	UPROPERTY(BlueprintAssignable, Category="Sequence")
	FOnGetEtherBalance EtherBalanceReceived;

	UPROPERTY(BlueprintAssignable, Category="Sequence")
	FOnGetTokenBalances TokenBalancesReceived;

	UPROPERTY(BlueprintAssignable, Category="Sequence")
	FOnGetTokenSupplies TokenSuppliesReceived;

	UPROPERTY(BlueprintAssignable, Category="Sequence")
	FOnGetTokenSuppliesMap TokenSuppliesMapReceived;

	UPROPERTY(BlueprintAssignable, Category="Sequence")
	FOnGetBalanceUpdates BalanceUpdatesReceived;

	UPROPERTY(BlueprintAssignable, Category="Sequence")
	FOnGetTransactionHistory TransactionHistoryReceived;

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void GetEtherBalanceAsync(int64 ChainId, FString WalletAddress);
	
	UFUNCTION(BlueprintCallable, Category="Sequence")
	void GetTokenBalancesAsync(const int64 ChainId, const FSeqGetTokenBalancesArgs& Args);

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void GetTokenSuppliesAsync(int64 ChainId, const FSeqGetTokenSuppliesArgs& Args);

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void GetTokenSuppliesMapAsync(int64 ChainId, const FSeqGetTokenSuppliesMapArgs& Args);

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void GetBalanceUpdatesAsync(int64 ChainId, const FSeqGetBalanceUpdatesArgs& Args);

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void GetTransactionHistoryAsync(int64 ChainId, const FSeqGetTransactionHistoryArgs& Args);
	
private:
	UPROPERTY()
	UIndexer* Indexer;
	
	static USequenceIndexerBP* GetSubSystem();

	void CallEtherBalanceReceived(const bool Status, const int64 Balance) const;
	void CallTokenBalancesReceived(const bool Status, const FSeqGetTokenBalancesReturn& Balances) const;
	void CallTokenSuppliesReceived(const bool Status, const FSeqGetTokenSuppliesReturn& Supplies) const;
	void CallTokenSuppliesMapReceived(const bool Status, const FSeqGetTokenSuppliesMapReturn& SuppliesMap) const;
	void CallBalanceUpdatesReceived(const bool Status, const FSeqGetBalanceUpdatesReturn& BalanceUpdates) const;
	void CallTransactionHistoryReceived(const bool Status, const FSeqGetTransactionHistoryReturn& TransactionHistory) const;
};
