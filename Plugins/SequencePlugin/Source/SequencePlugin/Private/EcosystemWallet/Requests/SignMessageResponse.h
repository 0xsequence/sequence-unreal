#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "SignMessageResponse.generated.h"

USTRUCT()
struct FSignMessageResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString Signature;

	UPROPERTY()
	FString WalletAddress;

	UPROPERTY()
	FString ManagerRequestId;

	inline static bool CustomJson = false;

	static FSignMessageResponse FromJson(const TSharedPtr<FJsonObject>& Json)
	{
		return FSignMessageResponse();
	}
};
