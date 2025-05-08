// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/Object.h"
#include "Util/Structs/BE_Enums.h"
#include "SequenceSdkBP.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API USequenceSdkBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USequenceSdkBP();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void SetChainById(const int64 NewChainId);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void SetChainByName(const FString& NewChainName);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void SetChainByType(const ENetwork& NewChainType);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void SetRedirectUrl(const FString& NewRedirectUrl);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	int64 GetChainId();
};
