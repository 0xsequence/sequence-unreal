#pragma once

#include "CoreMinimal.h"
#include "SardineRegionPaymentMethod.h"
#include "SardineRegionState.h"
#include "SardineRegion.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineRegion
{
	GENERATED_USTRUCT_BODY()

public:
	FString CountryCode;
	bool IsAllowedOnRamp;
	bool IsAllowedOnNFT;
	TArray<FString> IsBasicKycRequired;
	TArray<FString> IsSSNRequired;
	FString Name;
	FString CurrencyCode;
	bool IsPayrollSupported;
	TArray<FString> SupportedDocuments;
	TArray<FSardineRegionPaymentMethod> PaymentMethods;
	TArray<FSardineRegionState> States;
};