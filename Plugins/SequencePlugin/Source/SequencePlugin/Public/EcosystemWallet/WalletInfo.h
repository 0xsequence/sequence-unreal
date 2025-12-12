#pragma once

#include "CoreMinimal.h"
#include "WalletInfo.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct SEQUENCEPLUGIN_API FWalletInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="0xSequence SDK")
	FString Address;

	UPROPERTY(BlueprintReadOnly, Category="0xSequence SDK")
	FString Email;
};
