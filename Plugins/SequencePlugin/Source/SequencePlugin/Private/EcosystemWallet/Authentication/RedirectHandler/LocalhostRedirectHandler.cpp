#include "LocalhostRedirectHandler.h"
//#include "LocalhostServer.h"
#include "EcosystemWallet/Authentication/LocalhostListener.h"
#include "Engine/Engine.h"
#include "Misc/ScopeExit.h"
#include "Logging/LogMacros.h"
#include "Util/Log.h"

TOptional<FString> FLocalhostRedirectHandler::WaitForResponseImpl(const FString& FullUrl)
{
    FPlatformProcess::LaunchURL(*FullUrl, nullptr, nullptr);

    ULocalhostListener* Listener = NewObject<ULocalhostListener>();
    Listener->Start();

    /*ULocalhostServer* Server = ULocalhostServer::GetOrCreate();
    if (!Server)
    {
        SEQ_LOG_EDITOR(Error, TEXT("Failed to get/create LocalhostServer"));
        return MakeTuple(false, FString{});
    }

    TMap<FString, FString> Query;
    FString ServerError;

    const bool bOk = Server->Run(RedirectUrl, Query, ServerError);
    if (!bOk)
    {
        SEQ_LOG_EDITOR(Error, TEXT("Localhost server error: %s"), *ServerError);
        return MakeTuple(false, FString{});
    }

    const FString* IncomingId = Query.Find(TEXT("id"));
    if (!IncomingId || *IncomingId != Id)
    {
        SEQ_LOG_EDITOR(Error,
               TEXT("Incoming request id '%s' does not match expected id '%s'"),
               IncomingId ? **IncomingId : TEXT("<missing>"), *Id);
        return MakeTuple(false, FString{});
    }

    if (const FString* Err = Query.Find(TEXT("error")); Err && !Err->IsEmpty())
    {
        SEQ_LOG_EDITOR(Error, TEXT("Error during request: %s"), **Err);
        return MakeTuple(false, FString{});
    }

    const FString* PayloadB64 = Query.Find(TEXT("payload"));
    if (!PayloadB64)
    {
        SEQ_LOG_EDITOR(Error, TEXT("Missing 'payload' in query string"));
        return MakeTuple(false, FString{});
    }

    TArray<uint8> DecodedBytes;
    if (!FBase64::Decode(*PayloadB64, DecodedBytes))
    {
        SEQ_LOG_EDITOR(Error, TEXT("Failed to Base64-decode payload"));
        return MakeTuple(false, FString{});
    }

    DecodedBytes.Add(0);
    const char* Utf8Ptr = reinterpret_cast<const char*>(DecodedBytes.GetData());
    const FString ResponseJson = UTF8_TO_TCHAR(Utf8Ptr);

    SEQ_LOG_EDITOR(Display, TEXT("Response JSON: %s"), *ResponseJson);*/

    return TOptional<FString>("");
}
