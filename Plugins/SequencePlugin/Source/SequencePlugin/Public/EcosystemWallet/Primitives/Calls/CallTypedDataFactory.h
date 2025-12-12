#pragma once
#include "CallTypedData.h"
#include "JsonObjectConverter.h"

class SEQUENCEPLUGIN_API FCallTypedDataFactory
{
public:
	static FString FromCalls(const FString& Wallet, const FBigInt& ChainId, const TSharedPtr<FCalls>& Calls);

private:
	template <typename StructType>
	static TArray<TSharedPtr<FJsonValue>> StructArrayToJsonArray(const TArray<StructType>& Structs)
	{
		TArray<TSharedPtr<FJsonValue>> OutArray;
		for (const StructType& Elem : Structs)
		{
			TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(Elem);
			OutArray.Add(MakeShared<FJsonValueObject>(JsonObject));
		}
		return OutArray;
	}
};
