#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "TransactionOnRampProvider.generated.h"

ENUM_RANGE_BY_COUNT(ETransactionOnRampProvider, 3)
UENUM(Blueprintable)
enum class ETransactionOnRampProvider : uint8
{
	Unknown		UMETA(DisplayName = "unknown"),
	Sardine		UMETA(DisplayName = "sardine"),
	Transak		UMETA(DisplayName = "transak")
};

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UTransactionOnRampProvider : public UObject
{
	GENERATED_BODY()
	
public:
	static ETransactionOnRampProvider GetTransactionOnRampFromString(const FString& TransactionOnRampString)
	{
		FText EnumDisplayText = FText::FromString("");
		for (const ETransactionOnRampProvider Val : TEnumRange<ETransactionOnRampProvider>())
		{
			UEnum::GetDisplayValueAsText(Val, EnumDisplayText);
			
			if (UEnum::GetValueAsString(Val).Equals(TransactionOnRampString) || EnumDisplayText.ToString().Equals(TransactionOnRampString))
			{
				return Val;
			}
		}

		// Fallback if nothing is found
		constexpr ETransactionOnRampProvider TransactionOnRampDefault{ ETransactionOnRampProvider::Unknown };
		return TransactionOnRampDefault;
	}
	
	static FString AsString(const ETransactionOnRampProvider TransactionOnRampProvider)
	{
		switch (TransactionOnRampProvider)
		{
		case ETransactionOnRampProvider::Sardine: return "sardine";
		case ETransactionOnRampProvider::Transak: return "transak";
		default: return "unknown";
		}
	}
};
