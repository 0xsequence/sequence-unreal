// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "NativeOAuth.h"
#include "SequenceAuthenticator.h"
#include "AppleBridge.h"
#include "Async/TaskGraphInterfaces.h"
#include "Async/Async.h"
#include "Android/Support/AndroidSupport.h"

namespace NativeOAuth
{
	void RequestAuthWebView(const FString& requestUrl, const FString& redirectUrl, USequenceAuthenticator * AuthCallback)
	{
		Callback = AuthCallback;
#if PLATFORM_ANDROID
		AndroidThunkCpp_RequestAuthInWebView(requestUrl,redirectUrl);
#endif
	}
	
	void SignInWithGoogle(const FString& clientId, USequenceAuthenticator * AuthCallback)
	{
		Callback = AuthCallback;
#if PLATFORM_ANDROID
		AndroidThunkCpp_SignInWithGoogle(clientId);
#endif // PLATFORM_ANDROID
	}

	void SignInWithGoogle_IOS(const FString& Url, const FString& Scheme, USequenceAuthenticator * AuthCallback)
	{
		Callback = AuthCallback;
		UAppleBridge::InitiateGoogleSSO(Url,Scheme,ProcessIosTokenizedUrlCallback);
	}

	void ProcessIosTokenizedUrlCallback(char * tokenizedUrl)
	{
		const FString token = FString(UTF8_TO_TCHAR(tokenizedUrl));
		USequenceAuthenticator * CallbackLcl = Callback;
		AsyncTask(ENamedThreads::GameThread, [CallbackLcl,token]() {
			CallbackLcl->UpdateMobileLogin(token);
		});
	}
	
	void ProcessIosCallback(char * idToken)
	{
		const FString token = FString(UTF8_TO_TCHAR(idToken));
		USequenceAuthenticator * CallbackLcl = Callback;
		AsyncTask(ENamedThreads::GameThread, [CallbackLcl,token]() {
			CallbackLcl->UpdateMobileLogin_IdToken(token);
		});
	}
	
	void SignInWithApple(const FString& clientID, USequenceAuthenticator * AuthCallback)
	{
		Callback = AuthCallback;		
		UAppleBridge::InitiateIosSSO(clientID, ProcessIosCallback);
	}
	
#if PLATFORM_ANDROID
        void AndroidLog(const FString& message) {
//use dev flag here
    	 /*const FString marked_message = "[ACTIVE_UE_LOGGING]: " + message;
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
    	 	}*/
        }

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

	JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSequenceHandleSSOIdToken(JNIEnv * jenv, jobject thiz, jstring jIdToken)
    {
	    const char* idTokenChars = jenv->GetStringUTFChars(jIdToken, 0);
    	FString idToken = FString(UTF8_TO_TCHAR(idTokenChars));
    	Callback->UpdateMobileLogin_IdToken(idToken);
		jenv->ReleaseStringUTFChars(jIdToken, idTokenChars);
    }
	
JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSequenceHandleGoogleIdToken(JNIEnv* jenv, jobject thiz, jstring jIdToken)
    {
    	const char* idTokenChars = jenv->GetStringUTFChars(jIdToken, 0);
    	FString idToken = FString(UTF8_TO_TCHAR(idTokenChars));
    	Callback->UpdateMobileLogin_IdToken(idToken);
    	jenv->ReleaseStringUTFChars(jIdToken, idTokenChars);
    }


JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSequenceHandleRedirectUrl(JNIEnv* jenv, jobject thiz, jstring jRedirectUrl)
    {
    	const char* redirectUrlChars = jenv->GetStringUTFChars(jRedirectUrl, 0);
    	FString redirectUrl = FString(UTF8_TO_TCHAR(redirectUrlChars));
		USequenceAuthenticator * CallbackLcl = Callback;
		AsyncTask(ENamedThreads::GameThread, [CallbackLcl,redirectUrl]() {
			CallbackLcl->UpdateMobileLogin(redirectUrl);
		});
		
		jenv->ReleaseStringUTFChars(jRedirectUrl, redirectUrlChars);
    }
	
#endif
}
