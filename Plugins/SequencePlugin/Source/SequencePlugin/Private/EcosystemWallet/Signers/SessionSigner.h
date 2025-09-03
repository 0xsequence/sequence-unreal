#pragma once

#include "CoreMinimal.h"
#include "SessionCredentials.h"
#include "UObject/Object.h"
#include "SessionSigner.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API USessionSigner : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(const FSessionCredentials& NewCredentials);

private:
	UPROPERTY()
	FSessionCredentials Credentials;
};
