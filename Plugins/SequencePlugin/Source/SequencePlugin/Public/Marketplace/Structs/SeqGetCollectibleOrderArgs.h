#pragma once

#include "Util/JsonBuilder.h"
#include "SeqCollectiblesFilter.h"
#include "SeqGetCollectibleOrderArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetCollectibleOrderArgs
{
	GENERATED_USTRUCT_BODY()

	public:
	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
	FString ContractAddress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
	FString TokenID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
	FSeqCollectiblesFilter Filter;

	bool customGetter = true;

	FString GetArgs()
	{
		FJsonBuilder Builder;

		Builder.AddString("contractAddress", ContractAddress);
		Builder.AddString("tokenID", TokenID);

		if (Filter.ContainsData())
		{// Check if the Filter has data and append its args if it does
			Builder.AddField("filter", Filter.GetArgs());
		}

		return Builder.ToString();
	}
};
