// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequenceRPCManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceSdkBP.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSetupFinished);

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceSdkBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	USequenceSdkBP();

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnSetupFinished SetupFinished;

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SetupAsync();
	
private:
	static USequenceSdkBP* GetSubSystem();

	void CallSetupFinished() const;
};
