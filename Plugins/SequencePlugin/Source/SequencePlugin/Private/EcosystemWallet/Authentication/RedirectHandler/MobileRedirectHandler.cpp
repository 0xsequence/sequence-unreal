#include "MobileRedirectHandler.h"
#include "SourceUri.h"

TSharedPtr<TSuccessCallback<FString>> FMobileRedirectHandler::StrongOnSuccess = nullptr;
TSharedPtr<FFailureCallback> FMobileRedirectHandler::StrongOnFailure = nullptr;

TWeakPtr<TSuccessCallback<FString>> FMobileRedirectHandler::CurrentOnSuccess;
TWeakPtr<FFailureCallback> FMobileRedirectHandler::CurrentOnFailure;

#if PLATFORM_IOS
extern "C" void OpenWalletApp(const char* urlCString);

extern "C"
void IOS_OnDeepLink(const char* link)
{
	FString Url(UTF8_TO_TCHAR(link));
	FMobileRedirectHandler::HandleResponse(Url);
}
#elif PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include <jni.h>

extern "C"
JNIEXPORT void JNICALL
Java_xyz_sequence_ChromeTabsPlugin_OnDeepLinkReceived(JNIEnv* env, jclass clazz, jstring jUrl)
{
	const char* utf = env->GetStringUTFChars(jUrl, nullptr);
	FString Url = UTF8_TO_TCHAR(utf);
	env->ReleaseStringUTFChars(jUrl, utf);

	AsyncTask(ENamedThreads::GameThread, [Url]()
	{
		FMobileRedirectHandler::HandleResponse(Url);
	});
}
#endif

void FMobileRedirectHandler::HandleResponse(const FString& Url)
{
	if (TSharedPtr<TSuccessCallback<FString>> Success = FMobileRedirectHandler::CurrentOnSuccess.Pin())
	{
		if (*Success)
		{
			UE::DatasmithImporter::FSourceUri Uri = UE::DatasmithImporter::FSourceUri(Url);
			TMap<FString, FString> QueryParams = Uri.GetQueryMap();

			if (QueryParams.Contains(TEXT("error")))
			{
				FString Error = QueryParams[TEXT("error")];
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
			(*Failure)(FSequenceError(EErrorType::RequestFail, Error));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid failure callback pointer"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failure callback expired"));
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
		jclass ChromeTabsPluginClass = FAndroidApplication::FindJavaClass("xyz/sequence/ChromeTabsPlugin");

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
