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

	void Setup(FJsonObject& json_in)
	{
		json_in.TryGetStringField(TEXT("countryCode"), CountryCode);
		json_in.TryGetBoolField(TEXT("isAllowedOnRamp"), IsAllowedOnRamp);
		json_in.TryGetBoolField(TEXT("isAllowedOnNFT"), IsAllowedOnNFT);
		json_in.TryGetStringArrayField(TEXT("isBasicKycRequired"), IsBasicKycRequired);
		json_in.TryGetStringArrayField(TEXT("isSSNRequired"), IsSSNRequired);
		json_in.TryGetStringField(TEXT("name"), Name);
		json_in.TryGetStringField(TEXT("currencyCode"), CurrencyCode);
		json_in.TryGetBoolField(TEXT("isPayrollSupported"), IsPayrollSupported);
		json_in.TryGetStringArrayField(TEXT("supportedDocuments"), SupportedDocuments);

		for(TSharedPtr<FJsonValue> Method : json_in.GetArrayField(TEXT("paymentMethods")))
		{
			FSardineRegionPaymentMethod PaymentMethod;
			PaymentMethod.Setup(*Method->AsObject());
			PaymentMethods.Add(PaymentMethod);
		}
		
		for(TSharedPtr<FJsonValue> State : json_in.GetArrayField(TEXT("states")))
		{
			FSardineRegionState RegionState;
			RegionState.Setup(*State->AsObject());
			States.Add(RegionState);
		}
	}
};