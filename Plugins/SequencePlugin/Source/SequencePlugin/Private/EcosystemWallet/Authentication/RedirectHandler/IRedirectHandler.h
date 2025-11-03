#pragma once

#include "CoreMinimal.h"
#include "Misc/Base64.h"
#include "Misc/Guid.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Dom/JsonObject.h"
#include "Util/SequenceSupport.h"

class SEQUENCEPLUGIN_API IRedirectHandler
{
public:
    IRedirectHandler()
        : Id(FString::Printf(TEXT("sequence-%s"), *FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens)))
    {}

    virtual ~IRedirectHandler() = default;

    void SetWorld(UWorld* InWorld)
    {
        this->World = InWorld;
    }

    void SetRedirectUrl(const FString& InRedirectUrl)
    {
        RedirectUrl = InRedirectUrl;
    }

    template<typename TPayload, typename TResponse>
    void WaitForResponse(const FString& Url, const FString& Action, const TPayload& Payload, TFunction<void(TResponse)> OnSuccess, FFailureCallback OnFailure)
    {
        FString PayloadJson;
        if (Payload.CustomJson)
        {
            PayloadJson = Payload.ToJson();
        }
        else
        {
            PayloadJson = USequenceSupport::StructToString(Payload);
        }
        
        if (PayloadJson.IsEmpty())
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to serialize payload to JSON"));
            OnFailure(FSequenceError(EErrorType::ResponseParseError, TEXT("Failed to serialize payload to JSON")));
        }

        const FString FullUrl = ConstructUrl(Url, Action, PayloadJson);

        const TSuccessCallback<FString> OnHandlerSuccess = [OnSuccess](const FString& Response)
        {
            TResponse ResponseData;
            if (TResponse::CustomJson)
            {
                TSharedPtr<FJsonObject> JsonObject = USequenceSupport::JsonStringToObject(Response);
                ResponseData = TResponse::FromJson(JsonObject);
            }
            else
            {
                ResponseData = USequenceSupport::JSONStringToStruct<TResponse>(Response);    
            }
            
            OnSuccess(ResponseData);
        };

        WaitForResponseImpl(FullUrl, OnHandlerSuccess, OnFailure);
    }

protected:
    virtual void WaitForResponseImpl(const FString& FullUrl, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure) = 0;

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

    static FString Base64EncodeUtf8(const FString& In)
    {
        FTCHARToUTF8 Utf8(*In);
        return FBase64::Encode(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
    }

    FString Id;
    FString RedirectUrl;
    UWorld* World = nullptr;
};
