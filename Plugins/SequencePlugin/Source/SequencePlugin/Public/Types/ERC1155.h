#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Sequence/Transactions.h"
#include "ERC1155.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SEQUENCEPLUGIN_API UERC1155 : public UObject
{
	GENERATED_BODY()

public: 
	UERC1155();

	UFUNCTION(BlueprintCallable, Category = "ERC1155")
	static UERC1155* Create(const FString& ContractAddress, const FString& Data);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence - ERC1155")
	FString ContractAddress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence - ERC1155")
	FString Data;

	UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC1155")
	FRawTransaction MakeGrantRoleTransaction(const FString& role, const FString& ToAddress);

	UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC1155")
	FRawTransaction MakeSetApprovalForAllTransaction(const FString& Opperator, bool Approved);

	UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC1155")
	FRawTransaction MakeMintTransaction(const FString& ToAddress, const int32 TokenId, const int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC1155")
	FRawTransaction MakeBatchMintTransaction(const FString& ToAddress, const TArray<int32>& TokenIds, const TArray<int32>& Amounts);

	UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC1155")
	FRawTransaction MakeBurnTransaction(const int32 TokenId, const int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC1155")
	FRawTransaction MakeBatchBurnTransaction(const TArray<int32>& TokenIds, const TArray<int32>& Amounts);

private:
	void Init(const FString& InContractAddress, const FString& InData);
};

