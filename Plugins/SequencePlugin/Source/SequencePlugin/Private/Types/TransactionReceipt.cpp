#include "Types/TransactionReceipt.h"

#include "HexUtility.h"

FTransactionReceipt JsonToTransactionReceipt(TSharedPtr<FJsonObject> Json)
{
	uint64 BlockNumber;
	uint64 TransactionIndex;
	uint64 CumulativeGasUsed;
	uint64 GasUsed;
	FString toAddress;
	FAddress To = FAddress::From("");
	bool isValid = Json->TryGetNumberField("blockNumber", BlockNumber);
	isValid = Json->TryGetNumberField("TransactionIndex", TransactionIndex);
	isValid = Json->TryGetNumberField("CumulativeGasUsed", CumulativeGasUsed);
	isValid = Json->TryGetNumberField("GasUsed", GasUsed);
	bool isContract = !(Json->TryGetStringField("to", toAddress));
	if(!isContract) To = FAddress::From(toAddress);
	FHash256 BlockHash = FHash256::From(Json->GetStringField("blockHash"));
	FHash256 TransactionHash = FHash256::From(Json->GetStringField("transactionHash"));
	FAddress From = FAddress::From(Json->GetStringField("from"));
	FAddress ContractAddress = FAddress::From(Json->GetStringField("ContractAddress"));
	FString Status = Json->GetStringField("status");
	return FTransactionReceipt{
		BlockHash, BlockNumber, TransactionHash, TransactionIndex, From, To, CumulativeGasUsed, GasUsed, ContractAddress, Status
	};

}
