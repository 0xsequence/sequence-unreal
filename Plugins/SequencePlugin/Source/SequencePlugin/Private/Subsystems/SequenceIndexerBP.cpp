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

void USequenceIndexerBP::GetNativeTokenBalance(const FString& WalletAddress, FOnGetNativeTokenBalance OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqEtherBalance> OnApiSuccess = [this, OnSuccess](const FSeqEtherBalance& EtherBalance)
	{
		OnSuccess.Execute(EtherBalance.balanceWei);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting ether balance: %s"), *Error.Message);
		OnFailure.Execute(Error.Message);
	};
	
	this->Indexer->GetEtherBalance(this->ChainId, WalletAddress, OnApiSuccess, OnApiFailure);
}

void USequenceIndexerBP::GetTokenBalances(const FString& WalletAddress, const FString& ContractAddress, const bool IncludeMetadata, FOnGetTokenBalances OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetTokenBalancesReturn> OnApiSuccess = [this, OnSuccess](const FSeqGetTokenBalancesReturn& TokenBalances)
	{
		OnSuccess.Execute(TokenBalances);
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

void USequenceIndexerBP::GetTokenSupplies(const FString& ContractAddress, const bool IncludeMetadata, FOnGetTokenSupplies OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetTokenSuppliesReturn> OnApiSuccess = [this, OnSuccess](const FSeqGetTokenSuppliesReturn& Supplies)
	{
		OnSuccess.Execute(Supplies);
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

void USequenceIndexerBP::GetTokenSuppliesMap(const FSeqGetTokenSuppliesMapArgs& Args, FOnGetTokenSuppliesMap OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetTokenSuppliesMapReturn> OnApiSuccess = [this, OnSuccess](const FSeqGetTokenSuppliesMapReturn& SuppliesMap)
	{
		OnSuccess.Execute(SuppliesMap);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting token supplies map: %s"), *Error.Message);
		OnFailure.Execute(Error.Message);
	};
		
	this->Indexer->GetTokenSuppliesMap(ChainId, Args, OnApiSuccess, OnApiFailure);
}

void USequenceIndexerBP::GetTransactionHistory(const FSeqGetTransactionHistoryArgs& Args, FOnGetTransactionHistory OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetTransactionHistoryReturn> OnApiSuccess = [this, OnSuccess](const FSeqGetTransactionHistoryReturn& TransactionHistory)
	{
		OnSuccess.Execute(TransactionHistory);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting transaction history: %s"), *Error.Message);
		OnFailure.Execute(Error.Message);
	};
		
	this->Indexer->GetTransactionHistory(this->ChainId, Args, OnApiSuccess, OnApiFailure);
}
