// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

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
			FString EK = "";

		//Key Length
		UPROPERTY(VisibleAnywhere, Category = Basic)
			int32 KL = 0;

		UStorableCredentials();
};
