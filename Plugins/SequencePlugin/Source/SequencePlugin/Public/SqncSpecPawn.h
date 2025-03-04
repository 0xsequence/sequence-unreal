// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "SequenceAuthenticator.h"
#include "GameFramework/Pawn.h"
#include "SqncSpecPawn.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API ASqncSpecPawn : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY()
	FCredentials_BE Credentials;
	
	UFUNCTION(BlueprintCallable, CATEGORY="0xSequence")
	void SystemReady();
};
