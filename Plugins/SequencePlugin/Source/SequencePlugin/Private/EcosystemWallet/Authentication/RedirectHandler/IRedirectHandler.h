#pragma once

#include "CoreMinimal.h"
#include "Misc/Base64.h"
#include "Misc/Guid.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Util/SequenceSupport.h"

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
    void WaitForResponse(const FString& Url, const FString& Action, const TPayload& Payload, TSuccessCallback<TResponse> OnSuccess, FFailureCallback OnFailure)
    {
        const FString& PayloadJson = USequenceSupport::StructToString(Payload);
        if (PayloadJson.IsEmpty())
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to serialize payload to JSON"));
            OnFailure(FSequenceError(EErrorType::ResponseParseError, TEXT("Failed to serialize payload to JSON")));
        }

        const FString FullUrl = ConstructUrl(Url, Action, PayloadJson);

        const TSuccessCallback<FString> OnHandlerSuccess = [OnSuccess](const FString& Response)
        {
            TResponse ResponseData = USequenceSupport::JSONStringToStruct<TResponse>(Response);
            OnSuccess(ResponseData);
        };

        WaitForResponseImpl(FullUrl, OnHandlerSuccess, OnFailure);
    }

protected:
    // Implement this in your platform-specific handler:
    // Given the fully constructed URL, perform the actual wait/IO and return (bSuccess, ResponseJsonString).
    virtual void WaitForResponseImpl(const FString& FullUrl, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure) = 0;

    // Helper to build the full URL with Base64-encoded UTF-8 JSON payload
    FString ConstructUrl(const FString& Url, const FString& Action, const FString& PayloadJson) const
    {
        const FString EncodedPayload = Base64EncodeUtf8(PayloadJson);

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
