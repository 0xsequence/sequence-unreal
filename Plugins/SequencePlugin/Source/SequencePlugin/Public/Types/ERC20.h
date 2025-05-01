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

	UFUNCTION(BlueprintCallable, Category = "ERC20")
	static UERC20* Create(const FString& ContractAddress);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC20")
	FString ContractAddress;

	UFUNCTION(BlueprintCallable, Category = "ERC20")
	FRawTransaction MakeGrantRoleTransaction(const FString& role, const FString& ToAddress);

	UFUNCTION(BlueprintCallable, Category = "ERC20")
	FRawTransaction MakeApproveTransaction(const FString& Spender, const int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "ERC20")
	FRawTransaction MakeMintTransaction(const FString& ToAddress, const int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "ERC20")
	FRawTransaction MakeBurnTransaction(const int32 Amount);

private:
	void Init(const FString& InContractAddress);
};
 
