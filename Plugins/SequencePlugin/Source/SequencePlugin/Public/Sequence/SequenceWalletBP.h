// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Indexer/Structs/Struct_Data.h"
#include "SequenceWalletBPTypes.h"
#include "Transactions.h"
#include "SignedMessage.h"
#include "TransactionResponse.h"
#include "SequenceWalletBP.generated.h"

//ASYNC Response Delegates//

//Api//

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiSignMessage, FSequenceResponseStatus, ResponseStatus, FSignedMessage, SignedMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiGetFilteredFeeOptions, FSequenceResponseStatus, ResponseStatus, const TArray<FFeeOption>&, FeeOptions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiGetUnFilteredFeeOptions, FSequenceResponseStatus, ResponseStatus, const TArray<FFeeOption>&, FeeOptions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiSendTransactionWtihFeeOption, FSequenceResponseStatus, ResponseStatus, FTransactionResponse, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiSendTransaction, FSequenceResponseStatus, ResponseStatus, FTransactionResponse, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiListSessions, FSequenceResponseStatus, ResponseStatus, const TArray<FSession>&, Sessions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiGetSupportedTransakCountries, FSequenceResponseStatus, ResponseStatus, const TArray<FSupportedCountry>&, SupportedCountries);

//Api//

//Indexer

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

//Indexer//

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

	//Api//
	
	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIApiSignMessage OnApiSignMessage;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIApiGetFilteredFeeOptions OnApiGetFilteredFeeOptions;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIApiGetUnFilteredFeeOptions OnApiGetUnFilteredFeeOptions;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIApiSendTransactionWtihFeeOption OnApiSendTransactionWithFeeOption;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIApiSendTransaction OnApiSendTransaction;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIApiListSessions OnApiListSessions;

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIApiGetSupportedTransakCountries OnApiGetSupportedTransakCountries;
	
	//Api//

	//Indexer//
	
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

	//Indexer//
	
	//ASYNC Response Bindable Delegates//
private:

	//ASYNC Response Internal Calls//

	//Api//

	void CallOnApiSignMessage(const FSequenceResponseStatus& Status, const FSignedMessage& SignedMessage) const;
	void CallOnApiGetFilteredFeeOptions(const FSequenceResponseStatus& Status, const TArray<FFeeOption>& FeeOptions) const;
	void CallOnApiGetUnFilteredFeeOptions(const FSequenceResponseStatus& Status, const TArray<FFeeOption>& FeeOptions) const;
	void CallOnApiSendTransactionWithFee(const FSequenceResponseStatus& Status, const FTransactionResponse& Response) const;
	void CallOnApiSendTransaction(const FSequenceResponseStatus& Status, const FTransactionResponse& Response) const;
	void CallOnApiListSessions(const FSequenceResponseStatus& Status, const TArray<FSession>& Sessions) const;
	void CallOnApiGetSupportedTransakCountries(const FSequenceResponseStatus& Status, const TArray<FSupportedCountry>& SupportedCountries) const;
	
	//Api//

	//Indexer//
	
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

	//Indexer//
	
	//ASYNC Response Internal Calls//
	
public:
	//SYNC//
	
	//SequenceApi//
	
	UFUNCTION(BlueprintCallable)
	FString ApiGetWalletAddress();

	UFUNCTION(BlueprintCallable)
	int64 ApiGetNetworkId();

	UFUNCTION(BlueprintCallable)
	void ApiUpdateNetworkId(int64 NewNetworkId);

	UFUNCTION(BlueprintCallable)
	void ApiUpdateProviderUrl(const FString& NewProviderUrl);
	
	UFUNCTION(BlueprintCallable)
	void ApiOpenTransakUrl(const FTransakSettings& Settings);

	UFUNCTION(BlueprintCallable)
	void ApiSignOut();

	//SequenceApi//
	
	//SYNC//
	
	//ASYNC//

	//SequenceApi//

	UFUNCTION(BlueprintCallable)
	void ApiSignMessage(const FString& Message);

	UFUNCTION(BlueprintCallable)
	void ApiGetFilteredFeeOptions(UTransactions * Transactions);

	UFUNCTION(BlueprintCallable)
	void ApiGetUnfilteredFeeOptions(UTransactions * Transactions);

	UFUNCTION(BlueprintCallable)
	void ApiSendTransactionWithFee(UTransactions * Transactions);

	UFUNCTION(BlueprintCallable)
	void ApiSendTransaction(UTransactions * Transactions);

	UFUNCTION(BlueprintCallable)
	void ApiListSessions();

	UFUNCTION(BlueprintCallable)
	void ApiGetSupportedTransakCountries();
	
	//SequenceApi//

	//Indexer//
	
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

	//Indexer//
	
	//ASYNC//
};
