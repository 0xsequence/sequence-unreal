#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Sequence/Transactions.h"
#include "ERC20.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SEQUENCEPLUGIN_API UERC20 : public UObject
{
	GENERATED_BODY()

public:

	UERC20();


	UERC20(FString ContractAddress);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence")
	FString ContractAddress;

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	FRawTransaction MakeGrantRoleTransaction(const FString& role, const FString& ToAddress);

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	FRawTransaction MakeApproveTransaction(const FString& Spender, const int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	FRawTransaction MakeMintTransaction(const FString& ToAddress, const int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	FRawTransaction MakeBurnTransaction(const int32 Amount);
};
 
