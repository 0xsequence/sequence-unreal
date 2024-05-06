#include "NativeEncryptors/Android/AndroidNativeEncryptor.h"

UAndroidNativeEncryptor::UAndroidNativeEncryptor()
{
	
}

FString UAndroidNativeEncryptor::Encrypt(const FString& StringIn)
{
	FString Result = "";
/*#if PLATFORM_ANDROID
	if (JNIEnv* jenv{FAndroidApplication::GetJavaEnv()})
	{
		jclass gameActivityClass{FAndroidApplication::FindJavaClass("com/epicgames/unreal/GameActivity")};
		jmethodID methodId{FJavaWrapper::FindStaticMethod(
			jenv,
			gameActivityClass,
			"AndroidThunkJava_AndroidNativeEncrypt",
			"(Ljava/lang/String;)V",
			false
		)};
		//jenv->CallStaticVoidMethod(gameActivityClass, methodId, ConvertToJavaString(jenv, StringIn));
		String JResult = jenv->CallStaticStringMethod(gameActivityClass, methodId, ConvetToJavaString(jenv, StringIn));
		const char* JResultChars = jenv->GetStringUTFChars(JResult, 0);
		Result = FString(UTF8_TO_TCHAR(JResultChars));
		jenv->DeleteLocalRef(gameActivityClass);
	}          
#endif*/
	return Result;
}

FString UAndroidNativeEncryptor::Decrypt(const FString& StringIn)
{
	FString Result = "";
	return Result;
}