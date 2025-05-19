#pragma once

#include "SeqSwapToken.h"
#include "SeqLifiSwapRoute.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqLifiSwapRoute
{
	GENERATED_USTRUCT_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "toChainId"))
    int64 ToChainId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "fromChainId"))
    float FromChainId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "fromTokens"))
    TArray<FSeqSwapToken> FromTokens;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "toTokens"))
    TArray<FSeqSwapToken> ToTokens;

	bool ContainsToken(const FString& Contract) const
	{
		for (const FSeqSwapToken& Token : ToTokens)
		{
			if (Token.Contract == Contract)
			{
				return true;
			}
		}

		return false;
	}
};
