#pragma once

#include "CoreMinimal.h"
#include "Util/Async.h"
#include "Indexer/IndexerTests.h"

void TestGetFriends(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
void testGetCoins(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);

void TestSequenceData(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);