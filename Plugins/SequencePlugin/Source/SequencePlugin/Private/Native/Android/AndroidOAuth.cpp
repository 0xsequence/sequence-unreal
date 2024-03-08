#include "Native/Android/AndroidOAuth.h"

#if PLATFORM_ANDROID
namespace AndroidOAuth {
    void AndroidThunkCpp_RequestAuthCode(const FString& providerUrl, const FString& redirectScheme) 
    {
        if (JNIEnv* jenv{FAndroidApplication::GetJavaEnv()})   
        {
            jclass gameActivityClass{FAndroidApplication::FindJavaClass("com/epicgames/unreal/GameActivity")};
            jmethodID methodId{FJavaWrapper::FindStaticMethod(
                jenv,
                gameActivityClass, 
                "AndroidThunkJava_RequestSequenceAuthCode", 
                "(Ljava/lang/String;Ljava/lang/String;)V", 
                false
            )};

            jenv->CallStaticVoidMethod(
                gameActivityClass, 
                methodId, 
                AndroidOAuth::ConvertToJavaString(jenv, providerUrl),
                AndroidOAuth::ConvertToJavaString(jenv, redirectScheme)
            );

            jenv->DeleteLocalRef(gameActivityClass);
        }
    }

    jstring ConvertToJavaString(JNIEnv* jenv, const FString& string) 
    {
        const jstring localString = jenv->NewStringUTF(TCHAR_TO_UTF8(*string));
        const jstring globalString{
            static_cast<jstring>(jenv->NewGlobalRef(localString))
        };
        jenv->DeleteLocalRef(localString);
        return globalString;
    }
}

JNI_METHOD void Java_com_epicgames_unreal_GameActivity_onSequenceAuthCodeResponse(JNIEnv* jenv, jobject thiz, jstring jResponseUrl)
{
    const char* responseUrlChars = jenv->GetStringUTFChars(jResponseUrl, 0);

    FString responseUrl;
    responseUrl = FString(UTF8_TO_TCHAR(responseUrlChars));

    jenv->ReleaseStringUTFChars(jResponseUrl, responseUrlChars);

    UE_LOG(LogTemp, Warning, TEXT("NativeOAuth: redirect response url from java: %s"), *responseUrl);
}
#endif // PLATFORM_ANDROID