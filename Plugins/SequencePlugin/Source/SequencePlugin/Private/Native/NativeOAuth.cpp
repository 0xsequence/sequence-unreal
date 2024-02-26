#include "NativeOAuth.h"
#if PLATFORM_ANDROID
#include "Native/Android/AndroidOAuth.h"
#endif // PLATFORM_ANDROID

namespace NativeOAuth {
    void RequestAuthCode(const FString& providerUrl, const FString& redirectScheme)
    {
        #if PLATFORM_ANDROID
        AndroidOAuth::AndroidThunkCpp_RequestAuthCode(providerUrl, redirectScheme);
        #endif // PLATFORM_ANDROID
    }
}