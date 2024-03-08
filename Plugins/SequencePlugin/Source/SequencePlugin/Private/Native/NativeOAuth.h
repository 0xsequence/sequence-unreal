#pragma once

#include "Authenticator.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif // PLATFORM_ANDROID

namespace NativeOAuth
{
#if PLATFORM_ANDROID
void AndroidThunkCpp_SignInWithGoogle(const FString& clientId);
jstring ConvertToJavaString(JNIEnv* jenv, const FString& string);
#endif // PLATFORM_ANDROID
	inline UAuthenticator * Callback = nullptr;
	void SignInWithGoogle(const FString& clientId,UAuthenticator * AuthCallback);
}