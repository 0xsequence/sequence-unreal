// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequenceRPCManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceSdkBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceSdkBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	USequenceSdkBP();

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void SetupAsync();
	
private:
	static USequenceSdkBP* GetSubSystem();
};
