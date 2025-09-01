#pragma once

#include "CoreMinimal.h"
#include "ParameterRule.h"
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
};
