#pragma once

#include "CoreMinimal.h"
#include "SessionCredentials.h"
#include "WalletSessions.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FWalletSessions
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	TArray<FSessionCredentials> Sessions;

	bool HasSessions() const
	{
		return Sessions.Num() > 0;
	}
};
