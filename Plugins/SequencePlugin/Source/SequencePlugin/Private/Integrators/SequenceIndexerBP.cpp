// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Integrators/SequenceIndexerBP.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Util/Log.h"

USequenceIndexerBP::USequenceIndexerBP()
{
	this->Indexer = NewObject<UIndexer>();
}

void USequenceIndexerBP::SetChainId(int64 NewChainId)
{
	ChainId = NewChainId;
}

void USequenceIndexerBP::GetEtherBalanceAsync(const FString& WalletAddress)
{
	const TSuccessCallback<FSeqEtherBalance> OnSuccess = [this](const FSeqEtherBalance& EtherBalance)
	{
		this->CallEtherBalanceReceived(true, EtherBalance.balanceWei);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting ether balance: %s"), *Error.Message);
		this->CallEtherBalanceReceived(false, 0);
	};
	
	this->Indexer->GetEtherBalance(this->ChainId, WalletAddress, OnSuccess, OnFailure);
}

void USequenceIndexerBP::GetTokenBalancesAsync(const FString& WalletAddress, const FString& ContractAddress, const bool IncludeMetadata)
{
	const TSuccessCallback<FSeqGetTokenBalancesReturn> OnSuccess = [this](const FSeqGetTokenBalancesReturn& TokenBalances)
	{
		this->CallTokenBalancesReceived(true, TokenBalances);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting token balances: %s"), *Error.Message);

		const FSeqGetTokenBalancesReturn Balances;
		this->CallTokenBalancesReceived(false, Balances);
	};

	FSeqGetTokenBalancesArgs Args;
	Args.accountAddress = WalletAddress;
	Args.contractAddress = ContractAddress;
	Args.includeMetaData = IncludeMetadata;
	
	this->Indexer->GetTokenBalances(this->ChainId, Args, OnSuccess, OnFailure);
}

void USequenceIndexerBP::GetTokenSuppliesAsync(const FString& ContractAddress, const bool IncludeMetadata)
{
	const TSuccessCallback<FSeqGetTokenSuppliesReturn> OnSuccess = [this](const FSeqGetTokenSuppliesReturn& Supplies)
	{
		this->CallTokenSuppliesReceived(true, Supplies);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting token supplies: %s"), *Error.Message);
		
		const FSeqGetTokenSuppliesReturn Supplies;
		this->CallTokenSuppliesReceived(false, Supplies);
	};

	FSeqGetTokenSuppliesArgs Args;
	Args.contractAddress = ContractAddress;
	Args.includeMetaData = IncludeMetadata;
		
	this->Indexer->GetTokenSupplies(this->ChainId, Args, OnSuccess, OnFailure);
}

void USequenceIndexerBP::GetTokenSuppliesMapAsync(const FSeqGetTokenSuppliesMapArgs& Args)
{
	const TSuccessCallback<FSeqGetTokenSuppliesMapReturn> OnSuccess = [this](const FSeqGetTokenSuppliesMapReturn& SuppliesMap)
	{
		this->CallTokenSuppliesMapReceived(true, SuppliesMap);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting token supplies map: %s"), *Error.Message);
		
		const FSeqGetTokenSuppliesMapReturn SuppliesMap;
		this->CallTokenSuppliesMapReceived(false, SuppliesMap);
	};
		
	this->Indexer->GetTokenSuppliesMap(ChainId, Args, OnSuccess, OnFailure);
}

void USequenceIndexerBP::GetTransactionHistoryAsync(const FSeqGetTransactionHistoryArgs& Args)
{
	const TSuccessCallback<FSeqGetTransactionHistoryReturn> OnSuccess = [this](const FSeqGetTransactionHistoryReturn& TransactionHistory)
	{
		this->CallTransactionHistoryReceived(true, TransactionHistory);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting transaction history: %s"), *Error.Message);
		
		const FSeqGetTransactionHistoryReturn TransactionHistory;
		this->CallTransactionHistoryReceived(true, TransactionHistory);
	};
		
	this->Indexer->GetTransactionHistory(this->ChainId, Args, OnSuccess, OnFailure);
}

void USequenceIndexerBP::CallEtherBalanceReceived(const bool Status, const int64 Balance) const
{
	if (this->EtherBalanceReceived.IsBound())
		this->EtherBalanceReceived.Broadcast(Status, Balance);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: EtherBalanceReceived"));
}

void USequenceIndexerBP::CallTokenBalancesReceived(const bool Status, const FSeqGetTokenBalancesReturn& Balances) const
{
	if (this->TokenBalancesReceived.IsBound())
		this->TokenBalancesReceived.Broadcast(Status, Balances);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: TokenBalancesReceived"));
}

void USequenceIndexerBP::CallTokenSuppliesReceived(const bool Status, const FSeqGetTokenSuppliesReturn& Supplies) const
{
	if (this->TokenSuppliesReceived.IsBound())
		this->TokenSuppliesReceived.Broadcast(Status, Supplies);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: TokenSuppliesReceived"));
}

void USequenceIndexerBP::CallTokenSuppliesMapReceived(const bool Status, const FSeqGetTokenSuppliesMapReturn& SuppliesMap) const
{
	if (this->TokenSuppliesMapReceived.IsBound())
		this->TokenSuppliesMapReceived.Broadcast(Status, SuppliesMap);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: TokenSuppliesMapReceived"));
}

void USequenceIndexerBP::CallTransactionHistoryReceived(const bool Status, const FSeqGetTransactionHistoryReturn& TransactionHistory) const
{
	if (this->TransactionHistoryReceived.IsBound())
		this->TransactionHistoryReceived.Broadcast(Status, TransactionHistory);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: TransactionHistoryReceived"));
}
