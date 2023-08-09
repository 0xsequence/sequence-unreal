#pragma once

#include "CoreMinimal.h"
#include "Async.h"

void TestSequenceData(TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);