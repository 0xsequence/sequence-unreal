#pragma once

#include "CoreMinimal.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif // PLATFORM_ANDROID

#if PLATFORM_ANDROID
	jstring ConvertToJavaString(JNIEnv* jenv, const FString& string);
	void JavaLog(const FString& message);
#endif

