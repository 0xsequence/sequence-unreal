// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "SequenceFederationSupport.generated.h"

UENUM(Blueprintable)
enum ESequenceLoginType
{
	Email UMETA(DisplayName = "Email"),
	OIDC_Google UMETA(DisplayName = "OIDC Google"),
	OIDC_Apple UMETA(DisplayName = "OIDC Apple"),
	PlayFab UMETA(DisplayName = "PlayFab"),
	Guest UMETA(DisplayName = "Guest")
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FFederationSupportData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Federation")
	FString Email = "";
	UPROPERTY(BlueprintReadWrite, Category="Federation")
	TArray<TEnumAsByte<ESequenceLoginType>> ValidLoginTypes;

	FFederationSupportData(){}

	FFederationSupportData(const FString& EmailIn, const TArray<TEnumAsByte<ESequenceLoginType>>& ValidLoginTypesIn)
	{
		Email = EmailIn;
		ValidLoginTypes = ValidLoginTypesIn;
	}
};