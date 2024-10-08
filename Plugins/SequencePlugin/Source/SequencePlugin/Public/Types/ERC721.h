#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Sequence/Transactions.h"
#include "ERC721.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SEQUENCEPLUGIN_API UERC721 : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC721")
	FString ContractAddress;

	UFUNCTION(BlueprintCallable, Category = "ERC721")
	FRawTransaction MakeGrantRoleTransaction(const FString& role, const FString& ToAddress);

	UFUNCTION(BlueprintCallable, Category = "ERC721")
	FRawTransaction MakeMintTransaction(const FString& ToAddress, const int32 TokenId);

	UFUNCTION(BlueprintCallable, Category = "ERC721")
	FRawTransaction MakeBurnTransaction(const int32 TokenId);

	UFUNCTION(BlueprintCallable, Category = "ERC721")
	FRawTransaction MakeBatchBurnTransaction(const TArray<int32>& TokenIds);
};

