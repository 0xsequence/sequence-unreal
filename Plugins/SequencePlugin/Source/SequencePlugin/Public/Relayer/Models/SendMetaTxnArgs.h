#pragma once

#include "CoreMinimal.h"
#include "IntentPrecondition.h"
#include "MetaTxn.h"
#include "SendMetaTxnArgs.generated.h"

USTRUCT(BlueprintType)
struct FSendMetaTxnArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FMetaTxn Call;
};
