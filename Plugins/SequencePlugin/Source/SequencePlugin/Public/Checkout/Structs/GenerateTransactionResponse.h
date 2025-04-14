#pragma once
#include "TransactionStep.h"
#include "Dom/JsonObject.h"
#include "GenerateTransactionResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGenerateTransactionResponse
{
	GENERATED_USTRUCT_BODY()

private:
	FString GetEachStepArg()
	{
		FString StepArgs = "";

		for (uint16 StepIndex = 0; StepIndex < Steps.Num(); StepIndex++)
		{
			if (StepIndex < Steps.Num() - 1)
			{
				StepArgs += Steps[StepIndex].GetArgs() + ",";
			}
			else
			{
				StepArgs += Steps[StepIndex].GetArgs();
			}
		}
		
		return StepArgs;
	}
	
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	TArray<FTransactionStep> Steps;
	
	FGenerateTransactionResponse()
	{
		this->Steps = TArray<FTransactionStep>();
	}

	FGenerateTransactionResponse(const TArray<FTransactionStep>& Steps)
	{
		this->Steps = Steps;
	}
	
	bool customConstructor = false;

	void Construct(FJsonObject JSON_In)
	{
	} 
	
	void Setup(const FJsonObject& JSON_In)
	{
		if (JSON_In.HasField(TEXT("steps")))
		{
			const TArray<TSharedPtr<FJsonValue>>& StepsArray = JSON_In.GetArrayField(TEXT("steps"));
			Steps.Empty();
			for (int32 i = 0; i < StepsArray.Num(); ++i) 
			{
				if (const TSharedPtr<FJsonValue> StepValue = StepsArray[i]; StepValue->Type == EJson::Object)
				{
					FTransactionStep Step;
					TSharedPtr<FJsonObject> StepJsonObject = StepValue->AsObject(); 
					Step.Setup(*StepJsonObject); 
					Steps.Add(Step); 
				}
			}
		}
	}
};
