
#include "AppleBridge.h"
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include "Native/IOS/objective_c/IOSOAuth.h"
#if PLATFORM_MAC
#include "Native/IOS/objective_c/NativeAppleEncryptor.mm"
#endif
#include "Native/IOS/objective_c/NativeAppleEncryptor.h"
#include "Util/Log.h"

using namespace std;

UAppleBridge::UAppleBridge() { }

FString UAppleBridge::AppleEncrypt(const FString& StringIn)
{
	FString Result = "";
#if PLATFORM_IOS || PLATFORM_MAC
	NSString * _ToEncrypt = StringIn.GetNSString();
	NativeAppleEncryptor * Encryptor = [[NativeAppleEncryptor alloc] init];
	char * CharResult = [Encryptor Encrypt:_ToEncrypt];
	Result = FString(UTF8_TO_TCHAR(CharResult));
#endif
	return Result;
}

FString UAppleBridge::AppleDecrypt(const FString& StringIn)
{
	FString Result = "";
#if PLATFORM_IOS || PLATFORM_MAC
	NSString * _ToDecrypt = StringIn.GetNSString();
	NativeAppleEncryptor * Encryptor = [[NativeAppleEncryptor alloc] init];
	char * CharResult = [Encryptor Decrypt:_ToDecrypt];
	Result = FString(UTF8_TO_TCHAR(CharResult));
#endif
	return Result;
}

void UAppleBridge::InitiateGoogleSSO(const FString& Url, const FString& Scheme, void(*IOSCallback)(char *idToken))
{
	SEQ_LOG(Display,TEXT("Preparing to Signin with Google"));
#if PLATFORM_IOS
	NSString * _url = Url.GetNSString();
	NSString * _scheme = Scheme.GetNSString();
	IOSOAuth * auth = [[IOSOAuth alloc] init];
	[auth InitGoogleSignin:_url Scheme:_scheme callback:^(char * idToken){
		IOSCallback(idToken);
	}];
#endif
}

void UAppleBridge::InitiateIosSSO(const FString& clientID, void(*IOSCallback)(char *idToken))
{
#if PLATFORM_IOS
	NSString * cID = clientID.GetNSString();
	IOSOAuth *auth = [[IOSOAuth alloc] init];
	[auth loadBrowserWithUrl:cID callback:^(char *idToken){
		IOSCallback(idToken);
	}];
#endif
}
