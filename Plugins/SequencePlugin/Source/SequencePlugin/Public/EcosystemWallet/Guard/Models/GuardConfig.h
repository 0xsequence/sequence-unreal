#pragma once

#include "CoreMinimal.h"

struct SEQUENCEPLUGIN_API FGuardConfig
{
	FString Url;
	TMap<FString, FString> ModuleAddresses;
};
