#include "EcosystemWallet/Transactions/TransactionBatch.h"
#include "EcosystemWallet/Primitives/Calls//Call.h"
#include "EcosystemWallet/Primitives/Calls/Calls.h"

UTransactionBatch* UTransactionBatch::CreateTransactionBatch(const TArray<TScriptInterface<ISeqTransactionBase>>& Transactions)
{
	UTransactionBatch* Transaction = NewObject<UTransactionBatch>();
	Transaction->TransactionBatch = Transactions;

	return Transaction;
}

FCalls UTransactionBatch::GetCalls()
{
	FCalls Calls;
	for (TScriptInterface<ISeqTransactionBase> Txn : TransactionBatch)
	{
		for (const FCalls InternalCalls = Txn.GetInterface()->GetCalls(); FCall Call : InternalCalls.Calls)
		{
			Calls.Calls.Add(Call);
		} 
	}
	
	return Calls;
}
