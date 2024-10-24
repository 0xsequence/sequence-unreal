// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequenceRPCManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceSessionsBP.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEmailLoginRequiresCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuthFailed, const FString&, Error);

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceSessionsBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	USequenceSessionsBP();
	
	UPROPERTY()
	FOnEmailLoginRequiresCode EmailLoginRequiresCode;

	UPROPERTY()
	FOnAuthFailed AuthFailed;
	
	UFUNCTION(BlueprintCallable, Category="Sequence/Login")
	void StartEmailLoginAsync(const FString& Email);

	UFUNCTION(BlueprintCallable, Category="Sequence/Login")
	void ConfirmEmailLoginWithCodeAsync(const FString& Code);

private:
	USequenceRPCManager* RPCManager;
	
	static USequenceSessionsBP* GetSubSystem();
};
