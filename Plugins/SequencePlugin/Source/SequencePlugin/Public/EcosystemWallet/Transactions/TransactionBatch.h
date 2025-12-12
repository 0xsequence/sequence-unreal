#pragma once

#include "CoreMinimal.h"
#include "Transaction.h"
#include "EcosystemWallet/Primitives/Calls/Calls.h"
#include "UObject/Object.h"
#include "TransactionBatch.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UTransactionBatch : public UObject, public ISeqTransactionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Transactions")
	static UTransactionBatch* CreateTransactionBatch(const TArray<TScriptInterface<ISeqTransactionBase>>& Transactions);
	
	virtual FCalls GetCalls() override;

	TArray<TScriptInterface<ISeqTransactionBase>> TransactionBatch;
};
