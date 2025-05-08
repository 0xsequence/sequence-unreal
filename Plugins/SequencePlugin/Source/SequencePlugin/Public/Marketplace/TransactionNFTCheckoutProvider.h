#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "TransactionNFTCheckoutProvider.generated.h"

ENUM_RANGE_BY_COUNT(ETransactionNftCheckoutProvider, 3)
UENUM(Blueprintable)
enum class ETransactionNftCheckoutProvider : uint8
{
	Unknown		UMETA(DisplayName = "unknown"),
	Sardine		UMETA(DisplayName = "sardine"),
	Transak		UMETA(DisplayName = "transak")
};

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UTransactionNftCheckoutProvider : public UObject
{
	GENERATED_BODY()
	
public:
	static ETransactionNftCheckoutProvider GetTransactionNftCheckoutFromString(const FString& TransactionNftCheckoutString)
	{
		FText EnumDisplayText = FText::FromString("");
		for (const ETransactionNftCheckoutProvider Val : TEnumRange<ETransactionNftCheckoutProvider>())
		{
			UEnum::GetDisplayValueAsText(Val, EnumDisplayText);
			
			if (UEnum::GetValueAsString(Val).Equals(TransactionNftCheckoutString) || EnumDisplayText.ToString().Equals(TransactionNftCheckoutString))
			{
				return Val;
			}
		}

		// Fallback if nothing is found
		constexpr ETransactionNftCheckoutProvider TransactionNftCheckoutDefault{ ETransactionNftCheckoutProvider::Unknown };
		return TransactionNftCheckoutDefault;
	}
	
	static FString AsString(const ETransactionNftCheckoutProvider TransactionNftCheckoutProvider)
	{
		switch (TransactionNftCheckoutProvider)
		{
		case ETransactionNftCheckoutProvider::Sardine: return "sardine";
		case ETransactionNftCheckoutProvider::Transak: return "transak";
		default: return "unknown";
		}
	}
};
