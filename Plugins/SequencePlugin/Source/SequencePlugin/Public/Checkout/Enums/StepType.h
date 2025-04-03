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
	
	static FString AsString(const EStepType StepValue)
	{
		switch (StepValue)
		{
			case EStepType::Unknown: return "unknown";
			case EStepType::TokenApproval: return "tokenApproval";
			case EStepType::Buy: return "buy";
			case EStepType::Sell: return "sell";
			case EStepType::CreateListing: return "createListing";
			case EStepType::CreateOffer: return "createOffer";
			case EStepType::SignEip712: return "signEIP712";
			case EStepType::SignEip191: return "signEIP191";
		default: return "unknown";
		}
	}
};

