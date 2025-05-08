#pragma once
#include "CoreMinimal.h"

UENUM()
enum EBlockTag
{
	ELatest UMETA(DisplayName = "latest"),
	EEarliest UMETA(DisplayName = "earliest"),
	EPending UMETA(DisplayName = "pending"),
	ESafe UMETA(DisplayName = "safe"),
	EFinalized UMETA(DisplayName = "finalized"),
};