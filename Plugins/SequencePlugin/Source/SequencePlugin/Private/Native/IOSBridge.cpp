// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "IOSBridge.h"
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include "Native/IOS/objective_c/IOSOAuth.h"
#include "Native/IOS/objective_c/IOSEncryptor.h"

using namespace std;

UIOSBridge::UIOSBridge() { }

FString UIOSBridge::IOSEncrypt(const FString& StringIn)
{
	FString Result = "";
#if PLATFORM_IOS
	NSString * _ToEncrypt = StringIn.GetNSString();
	IOSEncryptor * Encryptor = [[IOSEncryptor alloc] init];
	char * CharResult = [Encryptor Encrypt:_ToEncrypt];
	Result = FString(UTF8_TO_TCHAR(CharResult));
#endif
	return Result;
}

FString UIOSBridge::IOSDecrypt(const FString& StringIn)
{
	FString Result = "";
#if PLATFORM_IOS
	NSString * _ToDecrypt = StringIn.GetNSString();
	IOSEncryptor * Encryptor = [[IOSEncryptor alloc] init];
	char * CharResult = [Encryptor Decrypt:_ToDecrypt];
	Result = FString(UTF8_TO_TCHAR(CharResult));
#endif
	return Result;
}

void UIOSBridge::InitiateGoogleSSO(const FString& Url, const FString& Scheme, void(*IOSCallback)(char *idToken))
{
	UE_LOG(LogTemp,Display,TEXT("Preparing to Signin with Google"));
#if PLATFORM_IOS
	NSString * _url = Url.GetNSString();
	NSString * _scheme = Scheme.GetNSString();
	IOSOAuth * auth = [[IOSOAuth alloc] init];
	[auth InitGoogleSignin:_url Scheme:_scheme callback:^(char * idToken){
		IOSCallback(idToken);
	}];
#endif
}

void UIOSBridge::InitiateIosSSO(const FString& clientID, const FString& nonce, void(*IOSCallback)(char *idToken))
{
#if PLATFORM_IOS
	NSString * cID = clientID.GetNSString();
	NSString * n = nonce.GetNSString();
	IOSOAuth *auth = [[IOSOAuth alloc] init];
	[auth loadBrowserWithUrl:cID nonce:n callback:^(char *idToken){
		IOSCallback(idToken);
	}];
#endif
}
