#pragma once

#include "CoreMinimal.h"
#include "Misc/Base64.h"
#include "Misc/Guid.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"

class SEQUENCEPLUGIN_API IRedirectHandler
{
public:
    IRedirectHandler()
        : Id(FString::Printf(TEXT("sequence-%s"), *FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens)))
    {}

    virtual ~IRedirectHandler() = default;

    void SetRedirectUrl(const FString& InRedirectUrl)
    {
        RedirectUrl = InRedirectUrl;
    }

    /**
     * High-level API that mirrors: Task<(bool Result, TResponse Data)> WaitForResponse<TPayload,TResponse>(...)
     * - TPayload and TResponse should be UStruct types.
     * - This wrapper serializes payload -> JSON -> Base64, constructs the URL,
     *   calls the virtual impl to obtain response JSON, and deserializes into TResponse.
     *
     * Note: This is synchronous. If you need async, have your derived impl run work on a thread
     * and block here, or change the signature to use futures/delegates in your codebase.
     */
    template<typename TPayload, typename TResponse>
    TTuple<bool, TResponse> WaitForResponse(const FString& Url, const FString& Action, const TPayload& Payload)
    {
        FString PayloadJson;
        if (!FJsonObjectConverter::UStructToJsonObjectString(TPayload::StaticStruct(), &Payload, PayloadJson, 0, 0))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to serialize payload to JSON"));
            return MakeTuple(false, TResponse{});
        }

        // Build full URL (adds Base64-encoded payload, id, mode=redirect, redirectUrl)
        const FString FullUrl = ConstructUrl(Url, Action, PayloadJson);

        // Call platform-specific implementation to get (success, response-json)
        const TTuple<bool, FString> Raw = WaitForResponseImpl(FullUrl);

        // If impl failed, return default response
        if (!Raw.Get<0>())
        {
            return MakeTuple(false, TResponse{});
        }

        // Deserialize response JSON back into TResponse
        TResponse Response{};
        if (!FJsonObjectConverter::JsonObjectStringToUStruct<TResponse>(Raw.Get<1>(), &Response, 0, 0))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to deserialize response JSON"));
            return MakeTuple(false, TResponse{});
        }

        return MakeTuple(true, Response);
    }

protected:
    // Implement this in your platform-specific handler:
    // Given the fully constructed URL, perform the actual wait/IO and return (bSuccess, ResponseJsonString).
    virtual TTuple<bool, FString> WaitForResponseImpl(const FString& FullUrl) = 0;

    // Helper to build the full URL with Base64-encoded UTF-8 JSON payload
    FString ConstructUrl(const FString& Url, const FString& Action, const FString& PayloadJson) const
    {
        const FString EncodedPayload = Base64EncodeUtf8(PayloadJson);

        // NOTE: If your Action/RedirectUrl can contain reserved chars, consider URL-encoding them.
        return FString::Printf(
            TEXT("%s?action=%s&payload=%s&id=%s&mode=redirect&redirectUrl=%s"),
            *Url,
            *Action,
            *EncodedPayload,
            *Id,
            *RedirectUrl
        );
    }

    // Base64-encode a UTF-8 view of the input string
    static FString Base64EncodeUtf8(const FString& In)
    {
        FTCHARToUTF8 Utf8(*In);
        return FBase64::Encode(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
    }

protected:
    FString Id;
    FString RedirectUrl;
};