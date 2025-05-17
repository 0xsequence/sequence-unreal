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
   
    TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetStringField(TEXT("imageURL"), NftData.ImageURL);
    JsonObject->SetStringField(TEXT("nftName"), NftData.Name);
    JsonObject->SetStringField(TEXT("collectionAddress"), NftData.CollectionAddress);

    TArray<TSharedPtr<FJsonValue>> TokenIDArray;
    for (const FString& TokenID : NftData.TokenID)
    {
        TokenIDArray.Add(MakeShared<FJsonValueString>(TokenID));
    }
    JsonObject->SetArrayField(TEXT("tokenID"), TokenIDArray);

    TArray<TSharedPtr<FJsonValue>> PriceArray;
    for (double Price : NftData.Price)
    {
        double RoundedPrice = FMath::RoundToDouble(Price * 100000.0) / 100000.0;
        PriceArray.Add(MakeShared<FJsonValueNumber>(RoundedPrice));
    }
    JsonObject->SetArrayField(TEXT("price"), PriceArray);

    JsonObject->SetNumberField(TEXT("quantity"), static_cast<double>(NftData.Quantity));

    FString EnumStr = UEnum::GetValueAsString(NftData.Type).Replace(TEXT("ENFTType::"), TEXT(""));
    JsonObject->SetStringField(TEXT("nftType"), EnumStr);

    TArray<TSharedPtr<FJsonValue>> JsonArray;
    JsonArray.Add(MakeShared<FJsonValueObject>(JsonObject));

    FString JsonString;
    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonString);

    if (!FJsonSerializer::Serialize(JsonArray, Writer))
    {
        return TEXT("");
    }

    FString Base64Encoded = FBase64::Encode(JsonString);

    UE_LOG(LogTemp, Log, TEXT("Base64 Encoded (with padding): %s"), *Base64Encoded);

    FString UrlEncoded = FGenericPlatformHttp::UrlEncode(Base64Encoded);

    UE_LOG(LogTemp, Log, TEXT("URL Encoded: %s"), *UrlEncoded);

    return UrlEncoded;
}
