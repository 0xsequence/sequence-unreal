﻿#include "Encryptors/AndroidEncryptor.h"
#include "Native/Android/Support/AndroidSupport.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif // PLATFORM_ANDROID

FString UAndroidEncryptor::Encrypt(const FString& StringIn)
{
	FString Result = "";
#if PLATFORM_ANDROID	
	if (JNIEnv* jenv{FAndroidApplication::GetJavaEnv()})
	{
		jclass gameActivityClass{FAndroidApplication::FindJavaClass("com/epicgames/unreal/GameActivity")};
		jmethodID methodId{FJavaWrapper::FindStaticMethod(
			jenv,
			gameActivityClass, 
			"AndroidThunkJava_AndroidEncrypt", 
			"(Ljava/lang/String;)Ljava/lang/String;", 
			false
		)};

		jstring EncryptedString = (jstring)jenv->CallStaticObjectMethod(gameActivityClass, methodId, ConvertToJavaString(jenv, StringIn));
		const char* JResultChars = jenv->GetStringUTFChars(EncryptedString, 0);
		Result = FString(UTF8_TO_TCHAR(JResultChars));
		jenv->DeleteLocalRef(gameActivityClass);		
	}          
#endif
	return Result;
}

FString UAndroidEncryptor::Decrypt(const FString& StringIn)
{
	FString Result = "";
#if PLATFORM_ANDROID
	if (JNIEnv* jenv{FAndroidApplication::GetJavaEnv()})
	{
		jclass gameActivityClass{FAndroidApplication::FindJavaClass("com/epicgames/unreal/GameActivity")};
		jmethodID methodId{FJavaWrapper::FindStaticMethod(
			jenv,
			gameActivityClass, 
			"AndroidThunkJava_AndroidDecrypt",
			"(Ljava/lang/String;)Ljava/lang/String;", 
			false
		)};

		jstring EncryptedString = (jstring)jenv->CallStaticObjectMethod(gameActivityClass, methodId, ConvertToJavaString(jenv, StringIn));
		const char* JResultChars = jenv->GetStringUTFChars(EncryptedString, 0);
		Result = FString(UTF8_TO_TCHAR(JResultChars));
		jenv->DeleteLocalRef(gameActivityClass);
	}
#endif
	return Result;
}
