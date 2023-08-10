#pragma once

#include "CoreMinimal.h"
#include "Async.h"
#include "IndexerTests.h" //move testing constants to testingConstants.h
#include "SequenceData.h"

void TestGetFriends(USequenceData* sequence, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);
void testGetCoins(USequenceData* sequence, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);

void TestSequenceData(TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);