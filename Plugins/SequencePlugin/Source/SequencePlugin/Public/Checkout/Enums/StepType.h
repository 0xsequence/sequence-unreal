#pragma once
#include "StepType.generated.h"

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
		const FString EStepTypeString{ TEXT("EStepType") };

		if (const UEnum* StepTypeEnum{ FindObject<UEnum>(ANY_PACKAGE, *EStepTypeString, true) }) 
		{
			const int32 Index{ StepTypeEnum->GetIndexByNameString(StepTypeString) };

			// Check if the enum is valid
			if (const EStepType RegionInfoCheck{ static_cast<EStepType>(static_cast<uint8>(Index)) }; UEnum::GetValueAsString(RegionInfoCheck) != TEXT("None"))
			{
				return RegionInfoCheck;
			}
		}

		// Fallback if nothing is found
		constexpr EStepType StepType{ EStepType::Unknown };
		return StepType;
	}
};

