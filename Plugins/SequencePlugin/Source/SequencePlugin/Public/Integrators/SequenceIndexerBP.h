// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequenceRPCManager.h"
#include "Indexer/Indexer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceIndexerBP.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetEtherBalance, bool, Status, const FString&, WalletAddress, int64, Balance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnGetTokenBalances, bool, Status, const FString&, WalletAddress, const FString&, ContractAddress, FSeqGetTokenBalancesReturn, Balances);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetTokenSupplies, bool, Status, const FString&, ContractAddress, FSeqGetTokenSuppliesReturn, Supplies);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetTokenSuppliesMap, bool, Status, const FSeqGetTokenSuppliesMapArgs&, Request, FSeqGetTokenSuppliesMapReturn, SuppliesMap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetTransactionHistory, bool, Status, const FSeqGetTransactionHistoryArgs&, Request, FSeqGetTransactionHistoryReturn, TransactionHistory);

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
	void SetChain(const FString& ChainName);
	
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
	
	void CallEtherBalanceReceived(const bool Status, const FString& WalletAddress, const int64 Balance) const;
	void CallTokenBalancesReceived(const bool Status, const FString& WalletAddress, const FString& ContractAddress, const FSeqGetTokenBalancesReturn& Balances) const;
	void CallTokenSuppliesReceived(const bool Status, const FString& ContractAddress, const FSeqGetTokenSuppliesReturn& Supplies) const;
	void CallTokenSuppliesMapReceived(const bool Status, const FSeqGetTokenSuppliesMapArgs& Request, const FSeqGetTokenSuppliesMapReturn& SuppliesMap) const;
	void CallTransactionHistoryReceived(const bool Status, const FSeqGetTransactionHistoryArgs& Request, const FSeqGetTransactionHistoryReturn& TransactionHistory) const;
};
