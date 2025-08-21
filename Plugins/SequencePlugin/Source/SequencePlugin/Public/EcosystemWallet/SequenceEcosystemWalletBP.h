// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SequenceEcosystemWallet.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceEcosystemWalletBP.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API USequenceEcosystemWalletBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	DECLARE_DYNAMIC_DELEGATE(FOnSuccess);
	
	USequenceEcosystemWalletBP();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SignInWithGoogle();

	USequenceEcosystemWallet* GetSequenceEcosystemWallet() const;
	
	void SignMessage(const FString& Message);
};
