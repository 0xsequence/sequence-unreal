#pragma once

#include "CoreMinimal.h"
#include "SardinePaymentOption.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardinePaymentOption
{
	GENERATED_USTRUCT_BODY()

public:
	FString Name;
	double DailyLimit;
	double WeeklyLimit;
	double MonthlyLimit;
	double MaxAmount;
	double MinAmount;
	TArray<FString> SubTypes;
	FString Type;
	FString SubType;
	FString ProcessingTime;
};