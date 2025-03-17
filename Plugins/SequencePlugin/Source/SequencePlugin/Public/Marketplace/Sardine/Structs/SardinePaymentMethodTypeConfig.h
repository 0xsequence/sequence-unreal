#pragma once

#include "CoreMinimal.h"
#include "SardinePaymentMethod.h"
#include "Util/JsonBuilder.h"
#include "SardinePaymentMethodTypeConfig.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardinePaymentMethodTypeConfig
{
	GENERATED_USTRUCT_BODY()

public:
	TArray<ESardinePaymentMethod> Enabled;
	ESardinePaymentMethod DefaultPaymentMethod;

	bool customGetter = true;

	FString GetArgs()
	{
		FJsonBuilder Builder;

		FJsonArray Array = Builder.AddArray("enabled");
		for(const ESardinePaymentMethod Method : Enabled)
		{
			Array.AddString(UEnum::GetValueAsString(Method).ToLower());
		}
		Builder = *Array.EndArray();
		Builder.AddString(TEXT("defaultPaymentMethod"), UEnum::GetValueAsString(DefaultPaymentMethod).ToLower());
		
		return Builder.ToString();
	}
};