#pragma once
#include "Errors.h"

class SequenceAPITest
{
public:
	static void RunTest(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
};
