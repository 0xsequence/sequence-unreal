// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Sockets.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Authenticator.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UAuthenticator : public UObject
{
	GENERATED_BODY()
//vars
private:
	const uint16 WINDOWS_IPC_PORT = 52836;
	FString StateToken = TEXT("");
	FString Nonce = TEXT("");

	FString UrlScheme = TEXT("powered-by-sequence");
	//need to know how to override this / get this out of browser
	FString RedirectURL = TEXT("https://3d41-142-115-54-118.ngrok-free.app/");

	FString GoogleAuthURL = TEXT("https://accounts.google.com/o/oauth2/auth");
	FString GoogleClientID = TEXT("970987756660-35a6tc48hvi8cev9cnknp0iugv9poa23.apps.googleusercontent.com");

	FString FacebookAuthURL = TEXT("https://www.facebook.com/v18.0/dialog/oauth");
	FString FacebookClientID = TEXT("");//TODO still need this

	FString DiscordAuthURL = TEXT("https://discord.com/api/oauth2/authorize");
	FString DiscordClientID = TEXT("");//TODO still need this

	FString AppleAuthURL = TEXT("https://appleid.apple.com/auth/authorize");
	FString AppleClientID = TEXT("");//TODO still need this
	//funcs
public:
	UAuthenticator();

	void GoogleSignin();

	//still pending
	void FacebookSignin();
	void DiscordSignin();
	void AppleSignin();

	FString GetSigninURL();

	FString GetRedirectURL();
private:
	FString GenerateSigninURL(FString AuthURL, FString ClientID);
	void OpenBrowser(FString URL);
	void StartListener();
	bool ListenerCallback(FSocket* socket, const FIPv4Endpoint& endpoint);
};
