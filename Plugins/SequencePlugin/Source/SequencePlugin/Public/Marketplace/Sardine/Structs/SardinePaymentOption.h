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

	void Setup(FJsonObject& json_in)
	{
		json_in.TryGetStringField(TEXT("name"), Name);
		json_in.TryGetNumberField(TEXT("dailyLimit"), DailyLimit);
		json_in.TryGetNumberField(TEXT("weeklyLimit"), WeeklyLimit);
		json_in.TryGetNumberField(TEXT("monthlyLimit"), MonthlyLimit);
		json_in.TryGetNumberField(TEXT("maxAmount"), MaxAmount);
		json_in.TryGetNumberField(TEXT("minAmount"), MinAmount);
		json_in.TryGetStringArrayField(TEXT("subTypes"), SubTypes);
		json_in.TryGetStringField(TEXT("type"), Type);
		json_in.TryGetStringField(TEXT("subType"), SubType);
		json_in.TryGetStringField(TEXT("processingTime"), ProcessingTime);
	}
};