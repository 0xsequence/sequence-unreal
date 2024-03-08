#include "NativeOAuth.h"
#include "Authenticator.h"

namespace NativeOAuth
{
    void SignInWithGoogle(const FString& clientId, UAuthenticator * AuthCallback)
    {
    	Callback = AuthCallback;
        #if PLATFORM_ANDROID
        AndroidThunkCpp_SignInWithGoogle(clientId);
        #endif // PLATFORM_ANDROID
    }
#if PLATFORM_ANDROID
    	void AndroidThunkCpp_SignInWithGoogle(const FString& clientId) 
    	{
    		if (JNIEnv* jenv{FAndroidApplication::GetJavaEnv()})   
    		{
    			jclass gameActivityClass{FAndroidApplication::FindJavaClass("com/epicgames/unreal/GameActivity")};
    			jmethodID methodId{FJavaWrapper::FindStaticMethod(
					jenv,
					gameActivityClass, 
					"AndroidThunkJava_SequenceSignInWithGoogle", 
					"(Ljava/lang/String;)V", 
					false
				)};

    			jenv->CallStaticVoidMethod(
					gameActivityClass, 
					methodId, 
					ConvertToJavaString(jenv, clientId)
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

JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSequenceHandleGoogleIdToken(JNIEnv* jenv, jobject thiz, jstring jIdToken)
    {
    	const char* idTokenChars = jenv->GetStringUTFChars(jIdToken, 0);

    	FString idToken;
    	idToken = FString(UTF8_TO_TCHAR(idTokenChars));
    	jenv->ReleaseStringUTFChars(jIdToken, idTokenChars);
    	Callback->SocialLogin(idToken);
    	UE_LOG(LogTemp, Warning, TEXT("NativeOAuth: received google id token from java: %s"), *idToken);
    }
#endif // PLATFORM_ANDROID
}