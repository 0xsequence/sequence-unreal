#pragma once
#include "CoreMinimal.h"

struct FSequenceConfig
{	
	inline static FString WaaSTenantKey = "eyJwcm9qZWN0SWQiOjIsImVtYWlsUmVnaW9uIjoidXMtZWFzdC0yIiwiZW1haWxDbGllbnRJZCI6IjVncDltaDJmYnFiajhsNnByamdvNzVwMGY2IiwicnBjU2VydmVyIjoiaHR0cHM6Ly9uZXh0LXdhYXMuc2VxdWVuY2UuYXBwIn0=";

	inline static FString ProjectAccessKey = "EeP6AmufRFfigcWaNverI6CAAAAAAAAAA";

	inline static FString WaasVersion = "1.0.0";
};

struct FAuthenticatorConfig
{
	inline static FString UrlScheme = "powered-by-sequence";
	inline static FString RedirectURL = "https://0xsequence.github.io/demo-waas-auth/";

	inline static FString GoogleAuthURL = "https://accounts.google.com/o/oauth2/auth";
	inline static FString GoogleClientID = "970987756660-35a6tc48hvi8cev9cnknp0iugv9poa23.apps.googleusercontent.com";

	inline static FString FacebookAuthURL = "https://www.facebook.com/v18.0/dialog/oauth";
	inline static FString FacebookClientID = "";//TODO still need this

	inline static FString DiscordAuthURL = "https://discord.com/api/oauth2/authorize";
	inline static FString DiscordClientID = "";//TODO still need this

	inline static FString AppleAuthURL = "https://appleid.apple.com/auth/authorize";
	inline static FString AppleClientID = "com.horizon.sequence.waas";
};