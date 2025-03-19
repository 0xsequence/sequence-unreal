#pragma once
#include "StepType.generated.h"

ENUM_RANGE_BY_COUNT(EStepType, 8)
UENUM(Blueprintable)
enum class EStepType : uint8
{
	Unknown				UMETA(DisplayName = "unknown"),
	TokenApproval		UMETA(DisplayName = "tokenApproval"),
	Buy					UMETA(DisplayName = "buy"),
	Sell				UMETA(DisplayName = "sell"),
	CreateListing		UMETA(DisplayName = "createListing"),
	CreateOffer			UMETA(DisplayName = "createOffer"),
	SignEip712			UMETA(DisplayName = "signEIP712"),
	SignEip191			UMETA(DisplayName = "signEIP191"),
};

UCLASS()
class SEQUENCEPLUGIN_API UStepType : public UObject
{
	GENERATED_BODY()

public:
	static EStepType GetStepTypeFromString(const FString& StepTypeString)
	{
		FText EnumDisplayText = FText::FromString("");
		for (const EStepType Val : TEnumRange<EStepType>())
		{
			UEnum::GetDisplayValueAsText(Val, EnumDisplayText);
			
			if (UEnum::GetValueAsString(Val).Equals(StepTypeString) || EnumDisplayText.ToString().Equals(StepTypeString))
			{
				return Val;
			}
		}

		// Fallback if nothing is found
		constexpr EStepType StepType{ EStepType::Unknown };
		return StepType;
	}
};

