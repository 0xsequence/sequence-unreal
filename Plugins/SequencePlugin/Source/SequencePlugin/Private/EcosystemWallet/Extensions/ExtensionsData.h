#pragma once

#include "CoreMinimal.h"

struct SEQUENCEPLUGIN_API FExtensionsData
{
	FString Factory;
	FString Stage1;
	FString Stage2;
	FString CreationCode;
	FString Sessions;
	TArray<FString> GuardSigners;
};