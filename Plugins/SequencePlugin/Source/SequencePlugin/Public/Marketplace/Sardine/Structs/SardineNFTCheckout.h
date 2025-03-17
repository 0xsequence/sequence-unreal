#pragma once

#include "CoreMinimal.h"
#include "SardineNFTCheckout.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineNFTCheckout
{
	GENERATED_USTRUCT_BODY()

public:
	FString Token;
	FString ExpiresAt;
	FString OrderID;

	bool customConstructor = false;

	void construct(FJsonObject& json_in) {}
	void Setup(FJsonObject& json_in)
	{
		Token = json_in.GetStringField(TEXT("token"));
		ExpiresAt = json_in.GetStringField(TEXT("expiresAt"));
		OrderID = json_in.GetStringField(TEXT("orderID"));
	}
};