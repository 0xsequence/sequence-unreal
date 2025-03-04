// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Subsystems/SequenceIndexerBP.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Util/Log.h"

USequenceIndexerBP::USequenceIndexerBP()
{
	this->Indexer = NewObject<UIndexer>();
}

void USequenceIndexerBP::SetChainById(const int64 NewChainId)
{
	ChainId = NewChainId;
}

void USequenceIndexerBP::SetChainByName(const FString& NewChainName)
{
	ChainId = USequenceSupport::GetNetworkId(NewChainName);
}

void USequenceIndexerBP::SetChainByType(const ENetwork& NewChainType)
{
	ChainId = USequenceSupport::GetNetworkId(NewChainType);
}

void USequenceIndexerBP::GetEtherBalanceAsync(const FString& WalletAddress, FOnGetEtherBalance OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqEtherBalance> OnApiSuccess = [this, WalletAddress, OnSuccess](const FSeqEtherBalance& EtherBalance)
	{
		OnSuccess.Execute(WalletAddress, EtherBalance.balanceWei);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting ether balance: %s"), *Error.Message);
		OnFailure.Execute(Error.Message);
	};
	
	this->Indexer->GetEtherBalance(this->ChainId, WalletAddress, OnApiSuccess, OnApiFailure);
}

void USequenceIndexerBP::GetTokenBalancesAsync(const FString& WalletAddress, const FString& ContractAddress, const bool IncludeMetadata, FOnGetTokenBalances OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetTokenBalancesReturn> OnApiSuccess = [this, WalletAddress, ContractAddress, OnSuccess](const FSeqGetTokenBalancesReturn& TokenBalances)
	{
		OnSuccess.Execute(WalletAddress, ContractAddress, TokenBalances);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting token balances: %s"), *Error.Message);
		OnFailure.Execute(Error.Message);
	};

	FSeqGetTokenBalancesArgs Args;
	Args.accountAddress = WalletAddress;
	Args.contractAddress = ContractAddress;
	Args.includeMetaData = IncludeMetadata;
	
	this->Indexer->GetTokenBalances(this->ChainId, Args, OnApiSuccess, OnApiFailure);
}

void USequenceIndexerBP::GetTokenSuppliesAsync(const FString& ContractAddress, const bool IncludeMetadata, FOnGetTokenSupplies OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetTokenSuppliesReturn> OnApiSuccess = [this, ContractAddress, OnSuccess](const FSeqGetTokenSuppliesReturn& Supplies)
	{
		OnSuccess.Execute(ContractAddress, Supplies);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting token supplies: %s"), *Error.Message);
		
		const FSeqGetTokenSuppliesReturn Supplies;
		OnFailure.Execute(Error.Message);
	};

	FSeqGetTokenSuppliesArgs Args;
	Args.contractAddress = ContractAddress;
	Args.includeMetaData = IncludeMetadata;
		
	this->Indexer->GetTokenSupplies(this->ChainId, Args, OnApiSuccess, OnApiFailure);
}

void USequenceIndexerBP::GetTokenSuppliesMapAsync(const FSeqGetTokenSuppliesMapArgs& Args, FOnGetTokenSuppliesMap OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetTokenSuppliesMapReturn> OnApiSuccess = [this, Args, OnSuccess](const FSeqGetTokenSuppliesMapReturn& SuppliesMap)
	{
		OnSuccess.Execute(Args, SuppliesMap);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting token supplies map: %s"), *Error.Message);
		OnFailure.Execute(Error.Message);
	};
		
	this->Indexer->GetTokenSuppliesMap(ChainId, Args, OnApiSuccess, OnApiFailure);
}

void USequenceIndexerBP::GetTransactionHistoryAsync(const FSeqGetTransactionHistoryArgs& Args, FOnGetTransactionHistory OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetTransactionHistoryReturn> OnApiSuccess = [this, Args, OnSuccess](const FSeqGetTransactionHistoryReturn& TransactionHistory)
	{
		OnSuccess.Execute(Args, TransactionHistory);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting transaction history: %s"), *Error.Message);
		OnFailure.Execute(Error.Message);
	};
		
	this->Indexer->GetTransactionHistory(this->ChainId, Args, OnApiSuccess, OnApiFailure);
}
