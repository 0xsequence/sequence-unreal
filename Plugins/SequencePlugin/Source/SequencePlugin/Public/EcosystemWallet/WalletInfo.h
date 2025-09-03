#pragma once

#include "CoreMinimal.h"
#include "WalletInfo.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct SEQUENCEPLUGIN_API FWalletInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString Address;

	UPROPERTY(BlueprintReadOnly)
	FString Email;
};
