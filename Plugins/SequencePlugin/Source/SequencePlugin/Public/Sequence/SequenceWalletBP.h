// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Indexer/Structs/Struct_Data.h"
#include "SequenceWalletBPTypes.h"
#include "SequenceWalletBP.generated.h"

//ASYNC Response Delegates//

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerPing, FSequenceResponseStatus, ResponseStatus, bool, PingResponse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerVersion, FSequenceResponseStatus, ResponseStatus, FVersion, Version);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerRuntimeStatus, FSequenceResponseStatus, ResponseStatus, FRuntimeStatus, RuntimeStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerGetChainId, FSequenceResponseStatus, ResponseStatus, int64, ChainId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerGetEtherBalance, FSequenceResponseStatus, ResponseStatus, FEtherBalance, EtherBalance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerGetTokenBalances, FSequenceResponseStatus, ResponseStatus, FGetTokenBalancesReturn, TokenBalances);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerGetTokenSupplies, FSequenceResponseStatus, ResponseStatus, FGetTokenSuppliesReturn, TokenSupplies);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerGetTokenSuppliesMap, FSequenceResponseStatus, ResponseStatus, FGetTokenSuppliesMapReturn, TokenSuppliesMap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerGetBalanceUpdates, FSequenceResponseStatus, ResponseStatus, FGetBalanceUpdatesReturn, BalanceUpdates);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerGetTransactionHistory, FSequenceResponseStatus, ResponseStatus, FGetTransactionHistoryReturn, TransactionHistory);

//ASYNC Response Delegates//

/**
 * Used for exposing portions of the USequenceWallet
 * to Blueprints for ease of use.
 */
UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceWalletBP : public UObject
{
	GENERATED_BODY()
private:
	
public:
	USequenceWalletBP();

	//ASYNC Response Bindable Delegates//

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIIndexerPing OnIndexerPing;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIIndexerVersion OnIndexerVersion;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIIndexerRuntimeStatus OnIndexerRuntimeStatus;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIIndexerGetChainId OnIndexerGetChainId;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIIndexerGetEtherBalance OnIndexerGetEtherBalance;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIIndexerGetTokenBalances OnIndexerGetTokenBalances;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIIndexerGetTokenSupplies OnIndexerGetTokenSupplies;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIIndexerGetTokenSuppliesMap OnIndexerGetTokenSuppliesMap;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIIndexerGetBalanceUpdates OnIndexerGetBalanceUpdates;
	
	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIIndexerGetTransactionHistory OnIndexerGetTransactionHistory;
	
	//ASYNC Response Bindable Delegates//
private:

	//ASYNC Response Internal Calls//

	void CallOnIndexerPing(const FSequenceResponseStatus& Status, bool PingResponse) const;
	void CallOnIndexerVersion(const FSequenceResponseStatus& Status, const FVersion& Version) const;
	void CallOnIndexerRuntimeStatus(const FSequenceResponseStatus& Status, const FRuntimeStatus& RuntimeStatus) const;
	void CallOnIndexerGetChainId(const FSequenceResponseStatus& Status, int64 ChainId) const;
	void CallOnIndexerGetEtherBalance(const FSequenceResponseStatus& Status, const FEtherBalance& EtherBalance) const;
	void CallOnIndexerGetTokenBalances(const FSequenceResponseStatus& Status, const FGetTokenBalancesReturn& TokenBalances) const;
	void CallOnIndexerGetTokenSupplies(const FSequenceResponseStatus& Status, const FGetTokenSuppliesReturn& TokenSupplies) const;
	void CallOnIndexerGetTokenSuppliesMap(const FSequenceResponseStatus& Status, const FGetTokenSuppliesMapReturn& TokenSuppliesMap) const;
	void CallOnIndexerGetBalanceUpdates(const FSequenceResponseStatus& Status, const FGetBalanceUpdatesReturn& BalanceUpdates) const;
	void CallOnIndexerGetTransactionHistory(const FSequenceResponseStatus& Status, const FGetTransactionHistoryReturn& TransactionHistory) const;
	
	//ASYNC Response Internal Calls//
	
public:
	//SYNC//
	
	UFUNCTION(BlueprintCallable)
	FString GetWalletAddress();

	UFUNCTION(BlueprintCallable)
	int64 GetNetworkId();

	UFUNCTION(BlueprintCallable)
	void UpdateNetworkId(int64 NewNetworkId);

	UFUNCTION(BlueprintCallable)
	void UpdateProviderUrl(const FString& NewProviderUrl);
	
	UFUNCTION(BlueprintCallable)
	void OpenTransakUrl(const FTransakSettings& Settings);

	UFUNCTION(BlueprintCallable)
	void SignOut();
	
	//SYNC//
	
	//ASYNC//

	UFUNCTION(BlueprintCallable)
	void IndexerPing();

	UFUNCTION(BlueprintCallable)
	void IndexerVersion();

	UFUNCTION(BlueprintCallable)
	void IndexerRuntimeStatus();

	UFUNCTION(BlueprintCallable)
	void IndexerGetChainId();

	UFUNCTION(BlueprintCallable)
	void IndexerGetEtherBalance(const FString& WalletAddress);

	UFUNCTION(BlueprintCallable)
	void IndexerGetTokenBalances(const FGetTokenBalancesArgs& Args);

	UFUNCTION(BlueprintCallable)
	void IndexerGetTokenSupplies(const FGetTokenSuppliesArgs& Args);

	UFUNCTION(BlueprintCallable)
	void IndexerGetTokenSuppliesMap(const FGetTokenSuppliesMapArgs& Args);

	UFUNCTION(BlueprintCallable)
	void IndexerGetBalanceUpdates(const FGetBalanceUpdatesArgs& Args);

	UFUNCTION(BlueprintCallable)
	void IndexerGetTransactionHistory(const FGetTransactionHistoryArgs& Args);
	
	//ASYNC//
};
