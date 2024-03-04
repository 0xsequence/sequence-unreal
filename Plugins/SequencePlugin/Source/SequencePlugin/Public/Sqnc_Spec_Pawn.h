// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Authenticator.h"
#include "GameFramework/SpectatorPawn.h"
#include "Sqnc_Spec_Pawn.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API ASqnc_Spec_Pawn : public ASpectatorPawn
{
	GENERATED_BODY()

private:
	UPROPERTY()
	FCredentials_BE Credentials;
	
	
	UFUNCTION(BlueprintCallable, CATEGORY="FUNCTION")
	void SetupCredentials(FCredentials_BE CredentialsIn);
	
};
