// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Authenticator.h"
#include "Misc/Guid.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Common/TcpListener.h"

UAuthenticator::UAuthenticator()
{
	this->Nonce = FGuid::NewGuid().ToString();
	this->StateToken = FGuid::NewGuid().ToString();
}

void UAuthenticator::GoogleSignin()
{
	this->OpenBrowser(this->GenerateSigninURL(GoogleAuthURL,GoogleClientID));
}

FString UAuthenticator::GenerateSigninURL(FString AuthURL, FString ClientID)
{
	//watch for trailing /'s in redirectURL
	FString ret = AuthURL + "?response_type=id_token&client_id="+ClientID+"&redirect_uri="+this->RedirectURL + "&scope=openid+profile+email&state=" + this->StateToken + "&nonce=" + this->Nonce;
	UE_LOG(LogTemp, Display, TEXT("Generated Signin URL: %s"), *ret);
	return ret;
}

void UAuthenticator::OpenBrowser(FString URL)
{
	FPlatformProcess::LaunchURL(*URL, NULL, NULL);
}

bool UAuthenticator::ListenerCallback(FSocket* socket, const FIPv4Endpoint& endpoint)
{
	uint32 payloadSize;
	TArray<uint8> buffer;
	FString result;
	if (socket->HasPendingData(payloadSize))
	{
		if (payloadSize < 8192 && payloadSize > 0)
		{
			buffer.SetNumUninitialized(payloadSize);
			int32 read = 0;
			socket->Recv(buffer.GetData(), buffer.Num(), read);
			result = BytesToString(buffer.GetData(), buffer.Num());
			UE_LOG(LogTemp, Display, TEXT("Response: %s"), *result);
			return true;
		}
	}
	return false;
}

void UAuthenticator::StartListener()
{
	FIPv4Address address;
	address.A = 1;
	address.B = 0;
	address.C = 0;
	address.D = 27;
	
	//is this platform specific it appears to be in the Unity project
	FIPv4Endpoint endpoint(address,this->WINDOWS_IPC_PORT);
	FTcpListener listener(endpoint);

	listener.OnConnectionAccepted().BindUObject(this,&UAuthenticator::ListenerCallback);//.BindLambda([&](FSocket* socket, const FIPv4Endpoint& endpoint)

	listener.Init();
	listener.Run();
}