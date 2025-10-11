#pragma once

#include "CoreMinimal.h"
#include "SessionCredentials.h"
#include "UObject/Object.h"

class SEQUENCEPLUGIN_API FSessionSigner
{
public:
	void Initialize(const FSessionCredentials& NewCredentials);

private:
	FSessionCredentials Credentials;
};
