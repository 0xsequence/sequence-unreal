#pragma once

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"

namespace AndroidOAuth {
    void AndroidThunkCpp_RequestAuthCode(const FString& providerUrl, const FString& redirectScheme);
    jstring ConvertToJavaString(JNIEnv* jenv, const FString& string);
}
#endif // PLATFORM_ANDROID