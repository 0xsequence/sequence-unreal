// Fill out your copyright notice in the Description page of Project Settings.

#include "NativeEncryptors/Android/NativeAndroidEncryptor.h"
#include "Native/Android/Support/AndroidSupport.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif // PLATFORM_ANDROID

UNativeAndroidEncryptor::UNativeAndroidEncryptor() : UGenericNativeEncryptor()
{ }

FString UNativeAndroidEncryptor::Encrypt(const FString& StringIn)
{
	FString Result = "";
	UE_LOG(LogTemp,Display,TEXT("Preparing to encrypt on android"));
#if PLATFORM_ANDROID	
	if (JNIEnv* jenv{FAndroidApplication::GetJavaEnv()})
	{
		//jclass JavaClass = jenv->FindClass("AndroidEncryptor");
		jclass JavaClass = FAndroidApplication::FindJavaClass("com/Plugins/SequencePlugin/AndroidEncryptor");
		//jmethodID JavaMethod = jenv->GetMethodID(JavaClass,"encrypt","(Ljava/lang/String;)Ljava/lang/String;");
		//jstring EncryptedString = (jstring)jenv->CallObjectMethod(JavaClass,JavaMethod,ConvertToJavaString(jenv, StringIn));
		//const char* JResultChars = jenv->GetStringUTFChars(EncryptedString, 0);
		//Result = FString(UTF8_TO_TCHAR(JResultChars));
	}          
#endif
	UE_LOG(LogTemp,Display,TEXT("Encrypted Result: %s"),*Result);
	return Result;
}

FString UNativeAndroidEncryptor::Decrypt(const FString& StringIn)
{
	FString Result = "";
	UE_LOG(LogTemp,Display,TEXT("Preparing to decrypt on android"));
#if PLATFORM_ANDROID
	if (JNIEnv* jenv{FAndroidApplication::GetJavaEnv()})
	{
		//jclass JavaClass = jenv->FindClass("AndroidEncryptor");
		/*jmethodID JavaMethod = jenv->GetMethodID(JavaClass,"decrypt","(Ljava/lang/String;)Ljava/lang/String;");
		jstring EncryptedString = (jstring)jenv->CallObjectMethod(JavaClass,JavaMethod,ConvertToJavaString(jenv, StringIn));
		const char* JResultChars = jenv->GetStringUTFChars(EncryptedString, 0);
		Result = FString(UTF8_TO_TCHAR(JResultChars));*/
	}
#endif
	return Result;
}
