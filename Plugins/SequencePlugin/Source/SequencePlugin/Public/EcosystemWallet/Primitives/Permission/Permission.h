#pragma once

#include "CoreMinimal.h"
#include "ParameterRule.h"
#include "Util/ByteArrayUtils.h"
#include "Permission.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FPermission
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FString Target;

	UPROPERTY()
	TArray<FParameterRule> Rules;

	static bool Decode(const TArray<uint8>& Data, int32 Offset, FPermission& OutPermission, int32& OutConsumed)
	{
		if (Data.Num() < Offset + 21)
		{
			UE_LOG(LogTemp, Error, TEXT("Data too short for permission"));
			return false;
		}

		TArray<uint8> TargetBytes;
		TargetBytes.Append(&Data[Offset], 20);
		OutPermission.Target = FByteArrayUtils::BytesToHexString(TargetBytes);
		Offset += 20;

		uint8 RulesLength = Data[Offset];
		Offset += 1;

		const int32 RuleSize = 97;
		TArray<FParameterRule> Rules;
		Rules.Reserve(RulesLength);

		for (int32 i = 0; i < RulesLength; i++)
		{
			if (Data.Num() < Offset + RuleSize)
			{
				UE_LOG(LogTemp, Error, TEXT("Data too short for parameter rule %d"), i);
				return false;
			}

			TArray<uint8> RuleBytes;
			RuleBytes.Append(&Data[Offset], RuleSize);

			FParameterRule Rule;
			if (!FParameterRule::Decode(RuleBytes, Rule))
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to decode parameter rule %d"), i);
				return false;
			}

			Rules.Add(Rule);
			Offset += RuleSize;
		}

		OutPermission.Rules = Rules;
		OutConsumed = 20 + 1 + RuleSize * RulesLength;
		return true;
	}
};
