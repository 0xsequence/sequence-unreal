#pragma once

#include "CoreMinimal.h"
#include "Transaction.h"
#include "EcosystemWallet/Primitives/Calls/Calls.h"
#include "UObject/Object.h"
#include "ContractTransaction.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UContractTransaction : public UObject, public ISeqTransactionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Transactions")
	static UContractTransaction* CreateContractTransaction(const FString& To, const FString& Value, const FString& FunctionSignature, const TArray<FString> Values);
	
	virtual FCalls GetCalls() override;

	FString TxnTo;
	FString TxnValue;
	FString TxnFunctionSignature;
	TArray<FString> TxnValues;
};
