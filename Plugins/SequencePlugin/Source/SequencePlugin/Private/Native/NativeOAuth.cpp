#include "NativeOAuth.h"
#if PLATFORM_ANDROID
#include "Native/Android/AndroidOAuth.h"
#endif // PLATFORM_ANDROID

namespace NativeOAuth {
    void SignInWithGoogle(const FString& clientId)
    {
        #if PLATFORM_ANDROID
        AndroidOAuth::AndroidThunkCpp_SignInWithGoogle(clientId);
        #endif // PLATFORM_ANDROID
    }
}