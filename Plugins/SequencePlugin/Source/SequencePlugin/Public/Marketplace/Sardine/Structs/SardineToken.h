#pragma once

#include "CoreMinimal.h"
#include "SardineToken.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineToken
{
	GENERATED_USTRUCT_BODY()

public:
	FString Network;
	FString AssetSymbol;
	FString AssetName;
	FString ChainID;
	FString TokenName;
	FString Token;
	FString TokenAddress;

	void Setup(FJsonObject& json_in)
	{
		json_in.TryGetStringField(TEXT("network"), Network);
		json_in.TryGetStringField(TEXT("assetSymbol"), AssetSymbol);
		json_in.TryGetStringField(TEXT("assetName"), AssetName);
		json_in.TryGetStringField(TEXT("chainID"), ChainID);
		json_in.TryGetStringField(TEXT("tokenName"), TokenName);
		json_in.TryGetStringField(TEXT("token"), Token);
		json_in.TryGetStringField(TEXT("tokenAddress"), TokenAddress);
	}
};