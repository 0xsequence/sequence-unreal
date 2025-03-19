#pragma once

#include "CoreMinimal.h"
#include "SardineFiatCurrency.h"
#include "SardineGetSupportedFiatCurrenciesReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineGetSupportedFiatCurrenciesReturn
{
	GENERATED_USTRUCT_BODY()

	TArray<FSardineFiatCurrency> Currencies;

	bool customConstructor = false; // Flag to determine the constructor used

	// Dummy constructor method to maintain consistency
	void construct(FJsonObject json_in) {}

	/*
	* Handles edge cases with Unreal's JSON parsing
	*/
	void Setup(FJsonObject json_in)
	{
		const TArray<TSharedPtr<FJsonValue>>* CurrenciesJson;
		if (json_in.TryGetArrayField(TEXT("tokens"), CurrenciesJson))
		{
			Currencies.SetNum(CurrenciesJson->Num());

			for (int32 i = 0; i < CurrenciesJson->Num(); i++)
			{
				TSharedPtr<FJsonValue> Currency = (*CurrenciesJson)[i];
				if (Currency.IsValid())
				{
					TSharedPtr<FJsonObject> CurrencyObj = Currency->AsObject();
					Currencies[i].Setup(*CurrencyObj);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No  tokens  field found in the SardineGetSupportedFiatCurrencies response."));
		}
	}
};
