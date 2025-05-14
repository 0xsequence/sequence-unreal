#include "Subsystems/SequenceUtilityBP.h"
#include "Engine/Engine.h"
#include "Util/Log.h"
#include "HAL/PlatformApplicationMisc.h"
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

void USequenceUtilityBP::ClipboardCopy(const FString& Text)
{
	TArray<TCHAR> CharArray = Text.GetCharArray();
	const TCHAR* CharArrayPtr = CharArray.GetData();
	FPlatformApplicationMisc::ClipboardCopy(CharArrayPtr);
}

float USequenceUtilityBP::GetUserReadableAmountIntDecimals(int64 Amount, int64 Decimals)
{
	return USequenceSupport::GetUserReadableAmount(Amount,Decimals);
}

int64 USequenceUtilityBP::GetTransactionReadableAmountIntDecimals(float Amount, int64 Decimals)
{
	return USequenceSupport::GetSystemReadableAmount(Amount,Decimals);
}

int64 USequenceUtilityBP::GetNetworkIdFromName(const FString& NetworkNameIn)
{
	return USequenceSupport::GetNetworkId(NetworkNameIn);
}

int64 USequenceUtilityBP::GetNetworkIdFromNetworkEnum(const ENetwork& NetworkEnumIn)
{
	return USequenceSupport::GetNetworkId(NetworkEnumIn);
}

FString USequenceUtilityBP::GetNetworkNameFromId(const int64 NetworkIdIn)
{
	return USequenceSupport::GetNetworkName(NetworkIdIn);
}

FString USequenceUtilityBP::GetNetworkNameFromEnum(const ENetwork NetworkIn)
{
	return USequenceSupport::GetNetworkName(NetworkIn);
}

TArray<FIdNamePair> USequenceUtilityBP::GetAllNetworks()
{
	return USequenceSupport::GetAllNetworks();
}

TArray<FString> USequenceUtilityBP::GetAllNetworkNames()
{
	return USequenceSupport::GetAllNetworkNames();
}

TArray<int64> USequenceUtilityBP::GetAllNetworkIds()
{
	return USequenceSupport::GetAllNetworkIds();
}

void USequenceUtilityBP::MakeSequenceLog(const ESeqLogVerbosity Verbosity, const FString& Message)
{
	switch (Verbosity)
	{
	case ESeqLogVerbosity::Display:
		SEQ_LOG(Display, TEXT("%s"), *Message);
		break;
	case ESeqLogVerbosity::Warning:
		SEQ_LOG(Warning, TEXT("%s"), *Message);
		break;
	case ESeqLogVerbosity::Error:
		SEQ_LOG(Error, TEXT("%s"), *Message);
		break;
	}
}

void USequenceUtilityBP::MakeSequenceEditorLog(const ESeqLogVerbosity Verbosity, const FString& Message)
{
	switch (Verbosity)
	{
		case ESeqLogVerbosity::Display:
			SEQ_LOG_EDITOR(Display, TEXT("%s"), *Message);
			break;
		case ESeqLogVerbosity::Warning:
			SEQ_LOG_EDITOR(Warning, TEXT("%s"), *Message);
			break;
		case ESeqLogVerbosity::Error:
			SEQ_LOG_EDITOR(Error, TEXT("%s"), *Message);
			break;
	}
}
