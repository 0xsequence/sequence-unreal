#pragma once
#include "ParameterRule.h"

class SEQUENCEPLUGIN_API Permission
{
public:
	FString Target;
	TArray<ParameterRule*> Rules;
};
