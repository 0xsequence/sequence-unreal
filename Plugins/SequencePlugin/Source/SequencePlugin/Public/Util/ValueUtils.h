#pragma once

#include "CoreMinimal.h"

class SEQUENCEPLUGIN_API FValueUtils
{
public:
	static FString Int32ToString(const int32& InValue);

	static int32 StringToInt32(const FString& InValue);
};
