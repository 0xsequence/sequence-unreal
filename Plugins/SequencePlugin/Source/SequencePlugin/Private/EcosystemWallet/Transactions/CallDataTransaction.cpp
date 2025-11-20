#include "EcosystemWallet/Transactions/CallDataTransaction.h"
#include "EcosystemWallet/Primitives/Calls//Call.h"
#include "EcosystemWallet/Primitives/Calls/Calls.h"

UCallDataTransaction* UCallDataTransaction::CreateCallDataTransaction(const FString& To, const FString& Value, const FString& Data)
{
	UCallDataTransaction* Transaction = NewObject<UCallDataTransaction>();
	Transaction->TxnTo = To;
	Transaction->TxnValue = Value;
	Transaction->TxnData = Data;

	return Transaction;
}

FCalls UCallDataTransaction::GetCalls()
{
	const FCall Call {
		this->TxnTo,
		this->TxnData,
		FBigInt(this->TxnValue),
		FBigInt(this->TxnValue),
		false,
		false,
		"1"
	};
	
	FCalls Calls;
	Calls.Calls.Add(Call);

	return Calls;
}
