#pragma once

#include "CoreMinimal.h"
#include "SeqLogVerbosity.generated.h"

UENUM(BlueprintType)
enum ESeqLogVerbosity
{
	Display UMETA(DisplayName = "Display"),
	Warning UMETA(DisplayName = "Warning"),
	Error UMETA(DisplayName = "Error")
};
