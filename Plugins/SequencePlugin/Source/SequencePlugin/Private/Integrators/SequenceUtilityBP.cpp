// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Integrators/SequenceUtilityBP.h"

#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Sequence/Transactions.h"

USequenceUtilityBP::USequenceUtilityBP() { }

UTransactions* USequenceUtilityBP::ConstructSingleERC20Transaction(const FString& ContractAddress, const FString& RecipientAddress, const FString& Value)
{
	FERC20Transaction Transaction;
	Transaction.tokenAddress = ContractAddress;
	Transaction.to = RecipientAddress;
	Transaction.value = Value;
	
	UTransactions* Transactions = NewObject<UTransactions>();
	Transactions->AddERC20(Transaction);
	
	return Transactions;
}

UTransactions* USequenceUtilityBP::ConstructSingleERC721Transaction(const FString& ContractAddress, const FString& RecipientAddress, const FString& TokenId)
{
	FERC721Transaction Transaction;
	Transaction.safe = true;
	Transaction.tokenAddress = ContractAddress;
	Transaction.to = RecipientAddress;
	Transaction.id = TokenId;
	
	UTransactions* Transactions = NewObject<UTransactions>();
	Transactions->AddERC721(Transaction);
	
	return Transactions;
}

UTransactions* USequenceUtilityBP::ConstructSingleERC1155Transaction(const FString& ContractAddress, const FString& RecipientAddress, const FString& TokenId, const FString& Amount)
{
	FERC1155Transaction Transaction;
	Transaction.tokenAddress = ContractAddress;
	Transaction.to = RecipientAddress;

	FERC1155TxnValue Val;
	Val.id = TokenId;
	Val.amount = Amount;
	Transaction.vals.Add(Val);
	
	UTransactions* Transactions = NewObject<UTransactions>();
	Transactions->AddERC1155(Transaction);
	
	return Transactions;
}
