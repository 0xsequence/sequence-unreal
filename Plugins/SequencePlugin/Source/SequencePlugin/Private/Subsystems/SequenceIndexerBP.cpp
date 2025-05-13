// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Subsystems/SequenceIndexerBP.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Util/Log.h"
#include "Sequence/SequenceSdk.h"

USequenceIndexerBP::USequenceIndexerBP()
{
	this->Indexer = NewObject<USequenceIndexer>();
}

void USequenceIndexerBP::GetNativeTokenBalance(const FString& WalletAddress, FOnGetNativeTokenBalance OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqEtherBalance> OnApiSuccess = [this, OnSuccess](const FSeqEtherBalance& EtherBalance)
	{
		OnSuccess.ExecuteIfBound(EtherBalance.balanceWei);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting ether balance: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Indexer->GetNativeTokenBalance(SequenceSdk::GetChainId(), WalletAddress, OnApiSuccess, OnApiFailure);
}

void USequenceIndexerBP::GetTokenBalances(const FString& WalletAddress, const FString& ContractAddress, const FSeqPage Page, const bool IncludeMetadata, FOnGetTokenBalances OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetTokenBalancesReturn> OnApiSuccess = [this, OnSuccess](const FSeqGetTokenBalancesReturn& TokenBalances)
	{
		OnSuccess.ExecuteIfBound(TokenBalances.balances, TokenBalances.page);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting token balances: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	FSeqGetTokenBalancesArgs Args;
	Args.accountAddress = WalletAddress;
	Args.contractAddress = ContractAddress;
	Args.page = Page;
	Args.includeMetaData = IncludeMetadata;
	
	this->Indexer->GetTokenBalances(SequenceSdk::GetChainId(), Args, OnApiSuccess, OnApiFailure);
}

void USequenceIndexerBP::GetTokenSupplies(const FString& ContractAddress, const FSeqPage Page, const bool IncludeMetadata, FOnGetTokenSupplies OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetTokenSuppliesReturn> OnApiSuccess = [this, OnSuccess](const FSeqGetTokenSuppliesReturn& Supplies)
	{
		OnSuccess.ExecuteIfBound(Supplies.tokenIDs, Supplies.contractType, Supplies.page);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting token supplies: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	FSeqGetTokenSuppliesArgs Args;
	Args.contractAddress = ContractAddress;
	Args.page = Page;
	Args.includeMetaData = IncludeMetadata;
		
	this->Indexer->GetTokenSupplies(SequenceSdk::GetChainId(), Args, OnApiSuccess, OnApiFailure);
}

void USequenceIndexerBP::GetTokenSuppliesMap(const TMap<FString, FSeqTokenList>& TokenMap, const bool IncludeMetadata, FOnGetTokenSuppliesMap OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetTokenSuppliesMapReturn> OnApiSuccess = [this, OnSuccess](const FSeqGetTokenSuppliesMapReturn& SuppliesMap)
	{
		OnSuccess.ExecuteIfBound(SuppliesMap);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting token supplies map: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	FSeqGetTokenSuppliesMapArgs Args;
	Args.tokenMap = TokenMap;
	Args.includeMetaData = IncludeMetadata;
	
	this->Indexer->GetTokenSuppliesMap(SequenceSdk::GetChainId(), Args, OnApiSuccess, OnApiFailure);
}

void USequenceIndexerBP::GetTransactionHistory(const FSeqTransactionHistoryFilter& Filter, const FSeqPage Page, const bool IncludeMetadata, FOnGetTransactionHistory OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetTransactionHistoryReturn> OnApiSuccess = [this, OnSuccess](const FSeqGetTransactionHistoryReturn& TransactionHistory)
	{
		OnSuccess.ExecuteIfBound(TransactionHistory.transactions, TransactionHistory.page);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting transaction history: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	FSeqGetTransactionHistoryArgs Args;
	Args.filter = Filter;
	Args.page = Page;
	Args.includeMetaData = IncludeMetadata;
	
	this->Indexer->GetTransactionHistory(SequenceSdk::GetChainId(), Args, OnApiSuccess, OnApiFailure);
}

void USequenceIndexerBP::RuntimeStatus(FOnRuntimeStatus OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqRuntimeStatus> OnApiSuccess = [this, OnSuccess](const FSeqRuntimeStatus& Status)
	{
		OnSuccess.ExecuteIfBound(Status);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Runtime Status: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Indexer->RuntimeStatus(SequenceSdk::GetChainId(), OnApiSuccess, OnApiFailure);
}

void USequenceIndexerBP::Version(FOnVersion OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqVersion> OnApiSuccess = [this, OnSuccess](const FSeqVersion& Status)
	{
		OnSuccess.ExecuteIfBound(Status.webrpcVersion, Status.schemaVersion, Status.schemaHash, Status.appVersion);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Indexer Version: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Indexer->Version(SequenceSdk::GetChainId(), OnApiSuccess, OnApiFailure);
}

void USequenceIndexerBP::Ping(FOnPing OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<bool> OnApiSuccess = [this, OnSuccess](const bool Status)
	{
		OnSuccess.ExecuteIfBound(Status);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error during Ping: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Indexer->Ping(SequenceSdk::GetChainId(), OnApiSuccess, OnApiFailure);
}

USequenceIndexer* USequenceIndexerBP::GetSequenceIndexer() const
{
	return this->Indexer;
}
