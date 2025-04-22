#include "Checkout/Transak/TransakNFTDataEncoder.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Misc/Base64.h"

namespace TransakNFTDataEncoder
{
	FString Encode(const FTransakNFTData& NftData)
	{
		TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();

		JsonObject->SetStringField(TEXT("imageUrl"), NftData.ImageURL);
		JsonObject->SetStringField(TEXT("name"), NftData.Name);
		JsonObject->SetStringField(TEXT("collectionAddress"), NftData.CollectionAddress);

		TArray<TSharedPtr<FJsonValue>> TokenIDs;
		for (const FString& TokenID : NftData.TokenID)
		{
			TokenIDs.Add(MakeShared<FJsonValueString>(TokenID));
		}
		JsonObject->SetArrayField(TEXT("tokenId"), TokenIDs);

		TArray<TSharedPtr<FJsonValue>> Prices;
		for (float Price : NftData.Price)
		{
			Prices.Add(MakeShared<FJsonValueNumber>(Price));
		}
		JsonObject->SetArrayField(TEXT("price"), Prices);

		JsonObject->SetNumberField(TEXT("quantity"), NftData.Quantity);
		JsonObject->SetStringField(TEXT("type"), UEnum::GetValueAsString(NftData.Type));

		// Serialize to string
		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObject, Writer);

		// Base64 encode JSON
		return FBase64::Encode(OutputString);
	}
}
