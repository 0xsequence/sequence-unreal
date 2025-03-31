// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "INativeAuthCallback.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif // PLATFORM_ANDROID

namespace NativeOAuth
{
#if PLATFORM_ANDROID
	void AndroidThunkCpp_SignInWithGoogle(const FString& clientId);
	void AndroidThunkCpp_RequestAuthInWebView(const FString& requestUrl, const FString& redirectUrl);
	void AndroidLog(const FString& message);
#endif // PLATFORM_ANDROID
	inline INativeAuthCallback * Callback = nullptr;
	void SignInWithGoogle(const FString& clientId, INativeAuthCallback * AuthCallback);
	void SignInWithGoogle_IOS(const FString& Url, const FString& RedirectUri, INativeAuthCallback * AuthCallback);
	void RequestAuthWebView(const FString& requestUrl, const FString& redirectUrl, INativeAuthCallback * AuthCallback);
	void SignInWithApple(const FString& clientID, INativeAuthCallback * AuthCallback);
	FString GetIdTokenFromTokenizedUrl(const FString& TokenizedUrl);
	void ProcessIosTokenizedUrlCallback(char * tokenizedUrl);
	void ProcessIosCallback(char * idToken);
}