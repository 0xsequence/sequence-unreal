#pragma once

#include "CoreMinimal.h"
#include "Async.h"
#include "IndexerTests.h"

void TestGetFriends(TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);
void testGetCoins(TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);

void TestSequenceData(TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);