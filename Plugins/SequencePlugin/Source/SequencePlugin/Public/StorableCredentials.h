// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "StorableCredentials.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UStorableCredentials : public USaveGame
{
	GENERATED_BODY()
public:
		//Encrypted Key
		UPROPERTY(VisibleAnywhere, Category = Basic)
			FString EK;

		//Key Length
		UPROPERTY(VisibleAnywhere, Category = Basic)
			int32 KL;

		UStorableCredentials();
};
