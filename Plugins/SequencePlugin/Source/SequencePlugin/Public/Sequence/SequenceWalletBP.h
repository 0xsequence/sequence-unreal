// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Util/SequenceSupport.h"
#include "Indexer/Structs/Struct_Data.h"
#include "SequenceWalletBPTypes.h"
#include "Transactions.h"
#include "SequenceResponseIntent.h"
#include "SequenceWalletBP.generated.h"

//ASYNC Response Delegates//

//Api//

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiSignMessage, FSequenceResponseStatus, ResponseStatus, FSeqSignMessageResponse_Response, SignedMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiGetFilteredFeeOptions, FSequenceResponseStatus, ResponseStatus, const TArray<FFeeOption>&, FeeOptions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiGetUnFilteredFeeOptions, FSequenceResponseStatus, ResponseStatus, const TArray<FFeeOption>&, FeeOptions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiSendTransactionWtihFeeOption, FSequenceResponseStatus, ResponseStatus, FSeqTransactionResponse_Data, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiSendTransaction, FSequenceResponseStatus, ResponseStatus, FSeqTransactionResponse_Data, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiListSessions, FSequenceResponseStatus, ResponseStatus, const TArray<FSeqListSessions_Session>&, Sessions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiGetSupportedTransakCountries, FSequenceResponseStatus, ResponseStatus, const TArray<FSupportedCountry>&, SupportedCountries);

//Api//

//Indexer

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerPing, FSequenceResponseStatus, ResponseStatus, bool, PingResponse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerVersion, FSequenceResponseStatus, ResponseStatus, FSeqVersion, Version);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerRuntimeStatus, FSequenceResponseStatus, ResponseStatus, FSeqRuntimeStatus, RuntimeStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerGetChainId, FSequenceResponseStatus, ResponseStatus, int64, ChainId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerGetEtherBalance, FSequenceResponseStatus, ResponseStatus, FSeqEtherBalance, EtherBalance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerGetTokenBalances, FSequenceResponseStatus, ResponseStatus, FSeqGetTokenBalancesReturn, TokenBalances);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerGetTokenSupplies, FSequenceResponseStatus, ResponseStatus, FSeqGetTokenSuppliesReturn, TokenSupplies);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerGetTokenSuppliesMap, FSequenceResponseStatus, ResponseStatus, FSeqGetTokenSuppliesMapReturn, TokenSuppliesMap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerGetTransactionHistory, FSequenceResponseStatus, ResponseStatus, FSeqGetTransactionHistoryReturn, TransactionHistory);

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
	FOnIIndexerGetTransactionHistory OnIndexerGetTransactionHistory;

	//Indexer//
	
	//ASYNC Response Bindable Delegates//
private:

	//ASYNC Response Internal Calls//

	//Api//

	void CallOnApiSignMessage(const FSequenceResponseStatus& Status, const FSeqSignMessageResponse_Response& SignedMessage) const;
	void CallOnApiGetFilteredFeeOptions(const FSequenceResponseStatus& Status, const TArray<FFeeOption>& FeeOptions) const;
	void CallOnApiGetUnFilteredFeeOptions(const FSequenceResponseStatus& Status, const TArray<FFeeOption>& FeeOptions) const;
	void CallOnApiSendTransactionWithFee(const FSequenceResponseStatus& Status, const FSeqTransactionResponse_Data& Response) const;
	void CallOnApiSendTransaction(const FSequenceResponseStatus& Status, const FSeqTransactionResponse_Data& Response) const;
	void CallOnApiListSessions(const FSequenceResponseStatus& Status, const TArray<FSeqListSessions_Session>& Sessions) const;
	void CallOnApiGetSupportedTransakCountries(const FSequenceResponseStatus& Status, const TArray<FSupportedCountry>& SupportedCountries) const;
	
	//Api//

	//Indexer//
	
	void CallOnIndexerPing(const FSequenceResponseStatus& Status, bool PingResponse) const;
	void CallOnIndexerVersion(const FSequenceResponseStatus& Status, const FSeqVersion& Version) const;
	void CallOnIndexerRuntimeStatus(const FSequenceResponseStatus& Status, const FSeqRuntimeStatus& RuntimeStatus) const;
	void CallOnIndexerGetChainId(const FSequenceResponseStatus& Status, int64 ChainId) const;
	void CallOnIndexerGetEtherBalance(const FSequenceResponseStatus& Status, const FSeqEtherBalance& EtherBalance) const;
	void CallOnIndexerGetTokenBalances(const FSequenceResponseStatus& Status, const FSeqGetTokenBalancesReturn& TokenBalances) const;
	void CallOnIndexerGetTokenSupplies(const FSequenceResponseStatus& Status, const FSeqGetTokenSuppliesReturn& TokenSupplies) const;
	void CallOnIndexerGetTokenSuppliesMap(const FSequenceResponseStatus& Status, const FSeqGetTokenSuppliesMapReturn& TokenSuppliesMap) const;
	void CallOnIndexerGetTransactionHistory(const FSequenceResponseStatus& Status, const FSeqGetTransactionHistoryReturn& TransactionHistory) const;

	//Indexer//
	
	//ASYNC Response Internal Calls//
	
public:
	//SYNC//

	//Support//

	/*
	* Used to get the amount that users can read,
	* int64 Amount, the integer amount of Crypto
	* int64 Decimals, the integer amount of Decimal places used to represent the Amount
	* Return, The user readable Float amount
	*/
	UFUNCTION(BlueprintCallable, Category="Sync Api")
	float GetUserReadableAmountIntDecimals(int64 Amount, int64 Decimals);

	/*
	* Used to convert a user readable amount into a usable amount value for transactions,
	* float Amount, the user readable amount they wish to use, EG) 0.01 Eth
	* int64 Decimals, the int64 amount of Decimal places used to represent the Amount
	* Return, The transaction usable amount
	*/
	UFUNCTION(BlueprintCallable, Category="Sync Api")
	int64 GetTransactionReadableAmountIntDecimals(float Amount, int64 Decimals);

	UFUNCTION(BlueprintCallable, Category="Networks Utility")
	static int64 GetNetworkIdFromName(const FString& NetworkNameIn);

	UFUNCTION(BlueprintCallable, Category = "Networks Utility")
	static int64 GetNetworkIdFromNetworkEnum(const ENetwork& NetworkEnumIn);

	UFUNCTION(BlueprintCallable, Category="Networks Utility")
	static FString GetNetworkNameFromId(const int64 NetworkIdIn);

	UFUNCTION(BlueprintCallable, Category = "Networks Utility")
	static FString GetNetworkNameFromEnum(const ENetwork NetworkIdIn);

	UFUNCTION(BlueprintCallable, Category="Networks Utility")
	static TArray<FIdNamePair> GetAllNetworks();

	UFUNCTION(BlueprintCallable, Category="Networks Utility")
	static TArray<FString> GetAllNetworkNames();

	UFUNCTION(BlueprintCallable, Category="Networks Utility")
	static TArray<int64> GetAllNetworkIds();
	
	//Support//
	
	//SequenceApi//
	
	UFUNCTION(BlueprintCallable, Category="Sync Api")
	FString ApiGetWalletAddress();

	UFUNCTION(BlueprintCallable, Category="Sync Api")
	int64 ApiGetNetworkId();

	UFUNCTION(BlueprintCallable, Category="Sync Api")
	void ApiUpdateNetworkId(int64 NewNetworkId);

	UFUNCTION(BlueprintCallable, Category = "Sync Api")
	void ApiUpdateNetworkIdFromName(FString NewNetworkName);

	UFUNCTION(BlueprintCallable, Category = "Sync Api")
	void ApiUpdateNetworkIdFromEnum(ENetwork NewNetwork);

	UFUNCTION(BlueprintCallable, Category="Sync Api")
	void ApiUpdateProviderUrl(const FString& NewProviderUrl);
	
	UFUNCTION(BlueprintCallable, Category="Sync Api")
	void ApiOpenTransakUrl(const FTransakSettings& Settings);

	UFUNCTION(BlueprintCallable, Category="Sync Api")
	void ApiSignOut();

	//SequenceApi//
	
	//SYNC//
	
	//ASYNC//

	//SequenceApi//

	UFUNCTION(BlueprintCallable, Category="ASync Api")
	void ApiSignMessage(const FString& Message);

	UFUNCTION(BlueprintCallable, Category="ASync Api")
	void ApiGetFilteredFeeOptions(UTransactions * Transactions);

	UFUNCTION(BlueprintCallable, Category="ASync Api")
	void ApiGetUnfilteredFeeOptions(UTransactions * Transactions);

	UFUNCTION(BlueprintCallable, Category="ASync Api")
	void ApiSendTransactionWithFee(UTransactions * Transactions);

	UFUNCTION(BlueprintCallable, Category="ASync Api")
	void ApiSendTransaction(UTransactions * Transactions);

	UFUNCTION(BlueprintCallable, Category="ASync Api")
	void ApiListSessions();

	UFUNCTION(BlueprintCallable, Category="ASync Api")
	void ApiGetSupportedTransakCountries();
	
	//SequenceApi//

	//Indexer//
	
	UFUNCTION(BlueprintCallable, Category="Async Indexer")
	void IndexerPing();

	UFUNCTION(BlueprintCallable, Category="Async Indexer")
	void IndexerVersion();

	UFUNCTION(BlueprintCallable, Category="Async Indexer")
	void IndexerRuntimeStatus();

	UFUNCTION(BlueprintCallable, Category="Async Indexer")
	void IndexerGetChainId();

	UFUNCTION(BlueprintCallable, Category="Async Indexer")
	void IndexerGetEtherBalance(const FString& WalletAddress);

	UFUNCTION(BlueprintCallable, Category="Async Indexer")
	void IndexerGetTokenBalances(const FSeqGetTokenBalancesArgs& Args);

	UFUNCTION(BlueprintCallable, Category="Async Indexer")
	void IndexerGetTokenSupplies(const FSeqGetTokenSuppliesArgs& Args);

	UFUNCTION(BlueprintCallable, Category="Async Indexer")
	void IndexerGetTokenSuppliesMap(const FSeqGetTokenSuppliesMapArgs& Args);

	UFUNCTION(BlueprintCallable, Category="Async Indexer")
	void IndexerGetTransactionHistory(const FSeqGetTransactionHistoryArgs& Args);

	//Indexer//
	
	//ASYNC//
};
