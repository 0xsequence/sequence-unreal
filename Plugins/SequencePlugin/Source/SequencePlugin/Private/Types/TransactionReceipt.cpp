#include "Types/TransactionReceipt.h"

FTransactionReceipt JsonToTransactionReceipt(TSharedPtr<FJsonObject> Json)
{
	uint64 BlockNumber;
	uint64 TransactionIndex;
	uint64 CumulativeGasUsed;
	uint64 GasUsed;
	FString ToAddress;
	FAddress To = FAddress::From("");
	bool bIsValid = Json->TryGetNumberField("blockNumber", BlockNumber);
	bIsValid = Json->TryGetNumberField("TransactionIndex", TransactionIndex);
	bIsValid = Json->TryGetNumberField("CumulativeGasUsed", CumulativeGasUsed);
	bIsValid = Json->TryGetNumberField("GasUsed", GasUsed);
	bool bIsContract = !(Json->TryGetStringField("to", ToAddress));
	if(!bIsContract) To = FAddress::From(ToAddress);
	FHash256 BlockHash = FHash256::From(Json->GetStringField("blockHash"));
	FHash256 TransactionHash = FHash256::From(Json->GetStringField("transactionHash"));
	FAddress From = FAddress::From(Json->GetStringField("from"));
	FAddress ContractAddress = FAddress::From(Json->GetStringField("ContractAddress"));
	FString Status = Json->GetStringField("status");
	return FTransactionReceipt{
		BlockHash, BlockNumber, TransactionHash, TransactionIndex, From, To, CumulativeGasUsed, GasUsed, ContractAddress, Status
	};

}
