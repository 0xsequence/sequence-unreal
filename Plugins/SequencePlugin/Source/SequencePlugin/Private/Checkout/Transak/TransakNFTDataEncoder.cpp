#include "Checkout/Transak/TransakNFTDataEncoder.h"
#include "Serialization/JsonWriter.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Misc/Base64.h"
#include "GenericPlatform/GenericPlatformHttp.h"

FString FTransakNFTDataEncoder::Encode(const FTransakNFTData& NftData)
{
    // Build the JSON object for the single NFT
    TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetStringField(TEXT("imageURL"), NftData.ImageURL);
    JsonObject->SetStringField(TEXT("nftName"), NftData.Name);
    JsonObject->SetStringField(TEXT("collectionAddress"), NftData.CollectionAddress);

    // tokenID array
    TArray<TSharedPtr<FJsonValue>> TokenIDArray;
    for (const FString& TokenID : NftData.TokenID)
    {
        TokenIDArray.Add(MakeShared<FJsonValueString>(TokenID));
    }
    JsonObject->SetArrayField(TEXT("tokenID"), TokenIDArray);

    // price array with high-precision formatting
    TArray<TSharedPtr<FJsonValue>> PriceArray;
    for (double Price : NftData.Price)
    {
        double RoundedPrice = FMath::RoundToDouble(Price * 100000.0) / 100000.0;
        PriceArray.Add(MakeShared<FJsonValueNumber>(RoundedPrice));
    }
    JsonObject->SetArrayField(TEXT("price"), PriceArray);

    // quantity
    JsonObject->SetNumberField(TEXT("quantity"), static_cast<double>(NftData.Quantity));

    // nftType (strip enum prefix)
    FString EnumStr = UEnum::GetValueAsString(NftData.Type).Replace(TEXT("ENFTType::"), TEXT(""));
    JsonObject->SetStringField(TEXT("nftType"), EnumStr);

    // Wrap object in an array
    TArray<TSharedPtr<FJsonValue>> JsonArray;
    JsonArray.Add(MakeShared<FJsonValueObject>(JsonObject));

    // Serialize to minified (condensed) JSON
    FString JsonString;
    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonString);

    if (!FJsonSerializer::Serialize(JsonArray, Writer))
    {
        return TEXT("");
    }

    // Base64 encode the JSON string (already padded)
    FString Base64Encoded = FBase64::Encode(JsonString);

    // Log Base64 output for debugging
    UE_LOG(LogTemp, Log, TEXT("Base64 Encoded (with padding): %s"), *Base64Encoded);

    // URL encode the Base64 string
    FString UrlEncoded = FGenericPlatformHttp::UrlEncode(Base64Encoded);

    // Log URL encoded string
    UE_LOG(LogTemp, Log, TEXT("URL Encoded: %s"), *UrlEncoded);

    return UrlEncoded;
}
