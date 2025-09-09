#pragma once

#include "CoreMinimal.h"
#include "Transaction.h"
#include "UObject/Object.h"
#include "ContractTransaction.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UContractTransaction : public UObject, public ISeqTransactionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Transactions")
	static UContractTransaction* CreateContractTransaction(const FString& To, const int64 Value, const FString& FunctionSignature, const TArray<FString> Values);
	
	virtual FCall GetCall() override;

	FString To;
	int64 Value;
	FString FunctionSignature;
	TArray<FString> Values;
};
