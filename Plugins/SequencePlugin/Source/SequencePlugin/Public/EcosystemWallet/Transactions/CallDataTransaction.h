#pragma once

#include "CoreMinimal.h"
#include "Transaction.h"
#include "EcosystemWallet/Primitives/Calls/Calls.h"
#include "UObject/Object.h"
#include "CallDataTransaction.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UCallDataTransaction : public UObject, public ISeqTransactionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Transactions")
	static UCallDataTransaction* CreateCallDataTransaction(const FString& To, const FString& Value, const FString& Data);
	
	virtual FCalls GetCalls() override;

	FString TxnTo;
	FString TxnValue;
	FString TxnData;
};
