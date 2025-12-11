#include "Types/TransactionReceipt.h"

FTransactionReceipt JsonToTransactionReceipt(TSharedPtr<FJsonObject> Json)
{
	FString BlockNumber;
	uint64 TransactionIndex;
	FString CumulativeGasUsed;
	FString GasUsed;
	FString ToAddress;
	FAddress To = FAddress::From("");
	bool bIsValid = Json->TryGetStringField(TEXT("blockNumber"), BlockNumber);
	bIsValid = Json->TryGetNumberField(TEXT("TransactionIndex"), TransactionIndex);
	bIsValid = Json->TryGetStringField(TEXT("CumulativeGasUsed"), CumulativeGasUsed);
	bIsValid = Json->TryGetStringField(TEXT("GasUsed"), GasUsed);
	bool bIsContract = !(Json->TryGetStringField(TEXT("to"), ToAddress));
	if(!bIsContract) To = FAddress::From(ToAddress);
	FHash256 BlockHash = FHash256::From(Json->GetStringField(TEXT("blockHash")));
	FHash256 TransactionHash = FHash256::From(Json->GetStringField(TEXT("transactionHash")));
	FAddress From = FAddress::From(Json->GetStringField(TEXT("from")));
	FAddress ContractAddress = FAddress::From(Json->GetStringField(TEXT("ContractAddress")));
	FString Status = Json->GetStringField(TEXT("status"));
	return FTransactionReceipt{
		BlockHash, BlockNumber, TransactionHash, TransactionIndex, From, To, CumulativeGasUsed, GasUsed, ContractAddress, Status
	};

}
