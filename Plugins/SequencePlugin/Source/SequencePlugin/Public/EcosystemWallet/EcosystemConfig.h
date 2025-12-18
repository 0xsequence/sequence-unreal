#pragma once

#include "CoreMinimal.h"
#include "EcosystemConfig.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct SEQUENCEPLUGIN_API FEcosystemConfig
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="0xSequence SDK")
	FString Name;
	
	UPROPERTY(BlueprintReadOnly, Category="0xSequence SDK")
	FString Description;
	
	UPROPERTY(BlueprintReadOnly, Category="0xSequence SDK")
	FString Url;
	
	UPROPERTY(BlueprintReadOnly, Category="0xSequence SDK")
	TArray<FString> SupportedChains;
	
	UPROPERTY(BlueprintReadOnly, Category="0xSequence SDK")
	TArray<FString> EnabledProviders;
};
