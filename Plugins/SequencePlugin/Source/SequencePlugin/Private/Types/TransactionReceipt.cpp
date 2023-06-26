#include "Types/TransactionReceipt.h"

#include "HexUtility.h"

FTransactionReceipt JsonToTransactionReceipt(TSharedPtr<FJsonObject> Json)
{
	FHash256 BlockHash = FHash256::From(Json->GetStringField("blockHash"));
	uint64 BlockNumber = Json->GetIntegerField("blockNumber");
	FHash256 TransactionHash = FHash256::From(Json->GetStringField("transactionHash"));
	uint64 TransactionIndex = Json->GetIntegerField("transactionIndex");
	FAddress From = FAddress::From(Json->GetStringField("from"));
	FAddress To = FAddress::From(Json->GetStringField("to"));
	uint64 CumulativeGasUsed = Json->GetIntegerField("cumulativeGasUsed");
	uint64 GasUsed = Json->GetIntegerField("gasUsed");
	FAddress ContractAddress = FAddress::From(Json->GetStringField("ContractAddress"));
	FString Status = Json->GetStringField("status");
	
	return FTransactionReceipt{
		BlockHash, BlockNumber, TransactionHash, TransactionIndex, From, To, CumulativeGasUsed, GasUsed, ContractAddress, Status
	};
}
