#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "TransactionSwapProvider.generated.h"

ENUM_RANGE_BY_COUNT(ETransactionSwapProvider, 2)
UENUM(Blueprintable)
enum class ETransactionSwapProvider : uint8
{
	Unknown		UMETA(DisplayName = "unknown"),
	Zerox		UMETA(DisplayName = "zerox")
};

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UTransactionSwapProvider : public UObject
{
	GENERATED_BODY()
	
public:
	static ETransactionSwapProvider GetTransactionSwapFromString(const FString& TransactionSwapString)
	{
		FText EnumDisplayText = FText::FromString("");
		for (const ETransactionSwapProvider Val : TEnumRange<ETransactionSwapProvider>())
		{
			UEnum::GetDisplayValueAsText(Val, EnumDisplayText);
			
			if (UEnum::GetValueAsString(Val).Equals(TransactionSwapString) || EnumDisplayText.ToString().Equals(TransactionSwapString))
			{
				return Val;
			}
		}

		// Fallback if nothing is found
		constexpr ETransactionSwapProvider TransactionSwapDefault{ ETransactionSwapProvider::Unknown };
		return TransactionSwapDefault;
	}
	
	static FString AsString(const ETransactionSwapProvider TransactionSwapProvider)
	{
		switch (TransactionSwapProvider)
		{
		case ETransactionSwapProvider::Zerox: return "zerox";
		default: return "unknown";
		}
	}
};
