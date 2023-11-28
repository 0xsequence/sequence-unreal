// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Authenticator.h"
#include "Misc/Guid.h"

UAuthenticator::UAuthenticator()
{
	this->Nonce = FGuid::NewGuid().ToString();
	this->StateToken = FGuid::NewGuid().ToString();
}

void UAuthenticator::GoogleSignin()
{
	this->GenerateSigninURL(GoogleAuthURL,GoogleClientID);
}

FString UAuthenticator::GenerateSigninURL(FString AuthURL, FString ClientID)
{
	//watch for trailing /'s in redirectURL
	FString ret = AuthURL + "?response_type=id_token&client_id="+ClientID+"&redirect_url="+this->RedirectURL + "&scope=openid+profile+email&state=" + this->StateToken + "&nonce=" + this->Nonce;
	UE_LOG(LogTemp, Display, TEXT("Generated Signin URL: %s"), *ret);
	return ret;
}