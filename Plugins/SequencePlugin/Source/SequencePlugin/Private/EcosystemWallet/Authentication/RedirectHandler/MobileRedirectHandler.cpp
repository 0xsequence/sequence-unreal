#include "MobileRedirectHandler.h"

TSharedPtr<TSuccessCallback<FString>> FMobileRedirectHandler::StrongOnSuccess = nullptr;
TSharedPtr<FFailureCallback> FMobileRedirectHandler::StrongOnFailure = nullptr;

TWeakPtr<TSuccessCallback<FString>> FMobileRedirectHandler::CurrentOnSuccess;
TWeakPtr<FFailureCallback> FMobileRedirectHandler::CurrentOnFailure;

#if PLATFORM_IOS
extern "C" void OpenExternalBrowser(const char* urlCString);
extern "C" void OpenWalletApp(const char* urlCString);
extern "C" void CloseWalletApp();
#elif PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include <jni.h>
#include <android/log.h>

extern "C"
JNIEXPORT void JNICALL
Java_com_Plugins_SequencePlugin_ChromeTabsPlugin_OnDeepLinkReceived(JNIEnv* env, jclass clazz, jstring jUrl)
{
	const char* utf = env->GetStringUTFChars(jUrl, nullptr);
	FString Url = UTF8_TO_TCHAR(utf);
	env->ReleaseStringUTFChars(jUrl, utf);

	__android_log_print(ANDROID_LOG_DEBUG, "SequenceDeepLink", "JNI native received: %s", TCHAR_TO_UTF8(*Url));

	AsyncTask(ENamedThreads::GameThread, [Url]()
	{
		FMobileRedirectHandler::HandleResponse(Url);
	});
}
#endif

void FMobileRedirectHandler::HandleResponse(const FString& Url)
{
#if PLATFORM_IOS
	CloseWalletApp();
#endif
	
	if (TSharedPtr<TSuccessCallback<FString>> Success = FMobileRedirectHandler::CurrentOnSuccess.Pin())
	{
		if (*Success)
		{
			TMap<FString, FString> QueryParams = ConvertUrlToQueryMap(Url);

			if (QueryParams.Contains(TEXT("error")))
			{
				const FString EncodedError = QueryParams[TEXT("error")];
				const FString Error = USequenceSupport::DecodeBase64ToString(EncodedError);
				FMobileRedirectHandler::HandleError(Error);
				return;
			}

			const FString Payload = QueryParams[TEXT("payload")];
			const FString PayloadJson = USequenceSupport::DecodeBase64ToString(*Payload);
			
			(*Success)(PayloadJson);
		}
		else
		{
			FMobileRedirectHandler::HandleError(TEXT("Invalid success callback pointer"));
		}
	}
	else
	{
		FMobileRedirectHandler::HandleError(TEXT("Success callback expired"));
	}
}

void FMobileRedirectHandler::HandleError(const FString& Error)
{
	if (TSharedPtr<FFailureCallback> Failure = FMobileRedirectHandler::CurrentOnFailure.Pin())
	{
		if (*Failure)
		{
			UE_LOG(LogTemp, Error, TEXT("FMobileRedirectHandler::HandleError %s"), *Error);
			(*Failure)(FSequenceError(EErrorType::RequestFail, Error));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid failure callback pointer"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failure callback expired"));
	}
}

void FMobileRedirectHandler::WaitForResponseImpl(const FString& FullUrl, TSuccessCallback<FString> OnSuccess,
                                                 FFailureCallback OnFailure)
{
	StrongOnSuccess = MakeShared<TSuccessCallback<FString>>(OnSuccess);
	StrongOnFailure = MakeShared<FFailureCallback>(OnFailure);

	CurrentOnSuccess = StrongOnSuccess;
	CurrentOnFailure = StrongOnFailure;
	
#if PLATFORM_IOS
	FTCHARToUTF8 Converter(*FullUrl);
	OpenWalletApp(Converter.Get());
#elif PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass ChromeTabsPluginClass = FAndroidApplication::FindJavaClass("com/Plugins/SequencePlugin/ChromeTabsPlugin");

		jmethodID OpenUrlMethod = Env->GetStaticMethodID(
			ChromeTabsPluginClass,
			"openUrl",
			"(Ljava/lang/String;)V"
		);

		jstring jUrl = Env->NewStringUTF(TCHAR_TO_UTF8(*FullUrl));
		Env->CallStaticVoidMethod(ChromeTabsPluginClass, OpenUrlMethod, jUrl);
		Env->DeleteLocalRef(jUrl);
	}
#else
	SEQ_LOG(Error, TEXT("MobileRedirectHandler is not supported on this platform."));
#endif
}

TMap<FString, FString> FMobileRedirectHandler::ConvertUrlToQueryMap(const FString& Url)
{
	TMap<FString, FString> Result;

	FString QueryString;
	if (!Url.Split(TEXT("?"), nullptr, &QueryString))
	{
		return Result; // no query found
	}

	// Split by '&'
	TArray<FString> Pairs;
	QueryString.ParseIntoArray(Pairs, TEXT("&"), true);

	for (const FString& Pair : Pairs)
	{
		FString Key, Value;
		if (Pair.Split(TEXT("="), &Key, &Value))
		{
			// URL decode the value
			Value = FPlatformHttp::UrlDecode(Value);
			Result.Add(Key, Value);
		}
	}

	return Result;
}

void FMobileRedirectHandler::LogMessage(const FString& Message)
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass ChromeTabsPluginClass = FAndroidApplication::FindJavaClass("com/Plugins/SequencePlugin/ChromeTabsPlugin");

		jmethodID OpenUrlMethod = Env->GetStaticMethodID(
			ChromeTabsPluginClass,
			"logMessage",
			"(Ljava/lang/String;)V"
		);

		jstring jUrl = Env->NewStringUTF(TCHAR_TO_UTF8(*Message));
		Env->CallStaticVoidMethod(ChromeTabsPluginClass, OpenUrlMethod, jUrl);
		Env->DeleteLocalRef(jUrl);
	}
#endif
}
