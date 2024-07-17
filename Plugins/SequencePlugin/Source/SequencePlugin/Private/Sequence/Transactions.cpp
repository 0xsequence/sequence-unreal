// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Sequence/Transactions.h"

void UTransactions::AddERC20(const FERC20Transaction& Item)
{
	this->Transactions.Push(TransactionUnion(Item));
}

void UTransactions::AddERC721(const FERC721Transaction& Item)
{
	this->Transactions.Push(TransactionUnion(Item));
}

void UTransactions::AddERC1155(const FERC1155Transaction& Item)
{
	this->Transactions.Push(TransactionUnion(Item));
}

void UTransactions::AddRaw(const FRawTransaction& Item)
{
	this->Transactions.Push(TransactionUnion(Item));
}

void UTransactions::AddDelayed(const FDelayedTransaction& Item)
{
	this->Transactions.Push(TransactionUnion(Item));
}

void UTransactions::SetFee(const FFeeOption& Fee)
{
	this->FeeSet = true;
	this->CachedFee = Fee;
}

bool UTransactions::IsFeeSet() const
{
	return this->FeeSet;
}

FFeeOption UTransactions::GetFee()
{
	return this->CachedFee;
}

TArray<TransactionUnion> UTransactions::GetTransactions()
{
	return this->Transactions;
}
