// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EcosystemClient.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UEcosystemClient : public UObject
{
	GENERATED_BODY()

public:
	void CreateNewSession();
};
