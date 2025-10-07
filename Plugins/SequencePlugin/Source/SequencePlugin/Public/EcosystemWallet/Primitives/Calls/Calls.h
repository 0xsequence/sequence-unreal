#pragma once

#include "CoreMinimal.h"
#include "Call.h"
#include "Calls.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FCalls
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TArray<FCall> Calls;
};
