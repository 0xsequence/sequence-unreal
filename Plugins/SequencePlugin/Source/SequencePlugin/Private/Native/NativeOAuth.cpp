#include "NativeOAuth.h"
#include "Authenticator.h"
#include "IOSBridge.h"

namespace NativeOAuth
{
	void RequestAuthWebView(const FString& requestUrl, const FString& redirectUrl, UAuthenticator * AuthCallback)
	{
		Callback = AuthCallback;
#if PLATFORM_ANDROID
		AndroidThunkCpp_RequestAuthInWebView(requestUrl,redirectUrl);
#endif
	}
	
	void SignInWithGoogle(const FString& clientId, const FString& nonce, UAuthenticator * AuthCallback)
	{
		Callback = AuthCallback;
#if PLATFORM_ANDROID
		AndroidThunkCpp_SignInWithGoogle(clientId, nonce);
#endif // PLATFORM_ANDROID
	}


	void ProcessIosCallback(char * idToken)
	{
		const FString token = FString(UTF8_TO_TCHAR(idToken));
		Callback->SocialLogin(token);
	}
	
	void SignInWithApple(const FString& providerUrl, UAuthenticator * AuthCallback)
	{
		Callback = AuthCallback;		
		UIOSBridge::InitiateIosSSO(providerUrl,ProcessIosCallback);
	}
	
#if PLATFORM_ANDROID
        void AndroidLog(const FString& message) {
    	 const FString marked_message = "[ACTIVE_UE_LOGGING]: " + message;
    	 	if (JNIEnv* jenv{FAndroidApplication::GetJavaEnv()})   
    	 	{
    	 		jclass gameActivityClass{FAndroidApplication::FindJavaClass("com/epicgames/unreal/GameActivity")};
    	 		jmethodID methodId{FJavaWrapper::FindStaticMethod(
		 			jenv,
		 			gameActivityClass, 
		 			"AndroidThunkJava_SequenceLog", 
		 			"(Ljava/lang/String;)V", 
		 			false
		 		)};

    	 		jenv->CallStaticVoidMethod(
		 			gameActivityClass, 
		 			methodId, 
		 			ConvertToJavaString(jenv, marked_message)
		 		);

    	 		jenv->DeleteLocalRef(gameActivityClass);
    	 	}            
        }

    	void AndroidThunkCpp_SignInWithGoogle(const FString& clientId, const FString& nonce) 
    	{
    		if (JNIEnv* jenv{FAndroidApplication::GetJavaEnv()})   
    		{
    			jclass gameActivityClass{FAndroidApplication::FindJavaClass("com/epicgames/unreal/GameActivity")};
    			jmethodID methodId{FJavaWrapper::FindStaticMethod(
					jenv,
					gameActivityClass, 
					"AndroidThunkJava_SequenceSignInWithGoogle", 
					"(Ljava/lang/String;Ljava/lang/String;)V", 
					false
				)};

    			jenv->CallStaticVoidMethod(
					gameActivityClass, 
					methodId, 
					ConvertToJavaString(jenv, clientId),
					ConvertToJavaString(jenv, nonce)
				);

    			jenv->DeleteLocalRef(gameActivityClass);
    		}
    	}

        void AndroidThunkCpp_RequestAuthInWebView(const FString& requestUrl, const FString& redirectUrl)
        {
    		if (JNIEnv* jenv{FAndroidApplication::GetJavaEnv()})   
    		{
    			jclass gameActivityClass{FAndroidApplication::FindJavaClass("com/epicgames/unreal/GameActivity")};
    			jmethodID methodId{FJavaWrapper::FindStaticMethod(
					jenv,
					gameActivityClass, 
					"AndroidThunkJava_SequenceRequestAuthInWebView", 
					"(Ljava/lang/String;Ljava/lang/String;)V", 
					false
				)};

    			jenv->CallStaticVoidMethod(
					gameActivityClass, 
					methodId, 
					ConvertToJavaString(jenv, requestUrl),
					ConvertToJavaString(jenv, redirectUrl)
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

	JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSequenceHandleSSOIdToken(JNIEnv * jenv, jobject thiz, jstring jIdToken)
    {
	    const char* idTokenChars = jenv->GetStringUTFChars(jIdToken, 0);
    	FString idToken;
    	idToken = FString(UTF8_TO_TCHAR(idTokenChars));
    	jenv->ReleaseStringUTFChars(jIdToken, idTokenChars);
    	Callback->SocialLogin(idToken);
    }
	
JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSequenceHandleGoogleIdToken(JNIEnv* jenv, jobject thiz, jstring jIdToken)
    {
    	const char* idTokenChars = jenv->GetStringUTFChars(jIdToken, 0);

    	FString idToken;
    	idToken = FString(UTF8_TO_TCHAR(idTokenChars));
    	jenv->ReleaseStringUTFChars(jIdToken, idTokenChars);
    	Callback->SocialLogin(idToken);
    }


JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSequenceHandleRedirectUrl(JNIEnv* jenv, jobject thiz, jstring jRedirectUrl)
    {
    	const char* redirectUrlChars = jenv->GetStringUTFChars(jRedirectUrl, 0);

    	FString redirectUrl;
    	redirectUrl = FString(UTF8_TO_TCHAR(redirectUrlChars));
    	jenv->ReleaseStringUTFChars(jRedirectUrl, redirectUrlChars);

        //work with redirectUrl
		//call out to cpp unreal from here
		Callback->UpdateMobileLogin(redirectUrl);
    }
#endif // PLATFORM_ANDROID
}
