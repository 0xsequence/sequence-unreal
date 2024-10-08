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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC20 Sale Contract")
	FString ContractAddress;

	UFUNCTION(BlueprintCallable, Category = "ERC20 Sale Contract")
	FRawTransaction MakeGrantRoleTransaction(const FString& role, const FString& ToAddress);

	UFUNCTION(BlueprintCallable, Category = "ERC20 Sale Contract")
	FRawTransaction MakeMintTransaction(const FString& ToAddress, const int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "ERC20 Sale Contract")
	FRawTransaction MakeBurnTransaction(const int32 Amount);
};
 
