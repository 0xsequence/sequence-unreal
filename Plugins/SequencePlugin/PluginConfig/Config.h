// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "CoreMinimal.h"

struct FEncryptorConfig
{
	inline static FString Key = "aaaabbbbccccddddaaaabbbbccccdddd";
};

struct FSequenceConfig
{	
	inline static FString WaaSTenantKey = "eyJwcm9qZWN0SWQiOjU5LCJycGNTZXJ2ZXIiOiJodHRwczovL25leHQtd2Fhcy5zZXF1ZW5jZS5hcHAifQ==";

	inline static FString ProjectAccessKey = "AQAAAAAAAAA744iuRfvHtvhmxe3tfzontSw";

	inline static FString WaasVersion = "1.0.0";
};

struct FAuthenticatorConfig
{
	inline static FString UrlScheme = "powered-by-sequence";
	inline static FString RedirectURL = "https://dev2-api.sequence.app/oauth/callback";

	inline static FString GoogleAuthURL = "https://accounts.google.com/o/oauth2/auth";
	inline static FString GoogleClientID = "970987756660-35a6tc48hvi8cev9cnknp0iugv9poa23.apps.googleusercontent.com";

	inline static FString FacebookAuthURL = "https://www.facebook.com/v18.0/dialog/oauth";
	inline static FString FacebookClientID = "";

	inline static FString DiscordAuthURL = "https://discord.com/api/oauth2/authorize";
	inline static FString DiscordClientID = "";

	inline static FString AppleAuthURL = "https://appleid.apple.com/auth/authorize";
	inline static FString AppleClientID = "com.horizon.sequence.waas";
};