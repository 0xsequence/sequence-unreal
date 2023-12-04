// Fill out your copyright notice in the Description page of Project Settings.

#include "Authenticator.h"
#include "Misc/Guid.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Common/TcpListener.h"
#include "Sockets.h"

UAuthenticator::UAuthenticator()
{
	this->Nonce = FGuid::NewGuid().ToString();
	this->StateToken = FGuid::NewGuid().ToString();
}

void UAuthenticator::GoogleSignin()
{
	this->OpenBrowser(this->GenerateSigninURL(GoogleAuthURL,GoogleClientID));
}

FString UAuthenticator::GetSigninURL()
{
	return this->GenerateSigninURL(GoogleAuthURL, GoogleClientID);
}

FString UAuthenticator::GetRedirectURL()
{
	return this->RedirectURL;
}

FString UAuthenticator::GenerateSigninURL(FString AuthURL, FString ClientID)
{
	//watch for trailing /'s in redirectURL
	FString ret = AuthURL + TEXT("?response_type=id_token&client_id=")+ClientID + TEXT("&redirect_uri=") + this->RedirectURL + TEXT("&scope=openid+profile+email&state=") + this->StateToken + TEXT("&nonce=") + this->Nonce;
	UE_LOG(LogTemp, Display, TEXT("Generated Signin URL: %s"), *ret);
	return ret;
}

void UAuthenticator::OpenBrowser(FString URL)
{
	StartListener();
	FPlatformProcess::LaunchURL(*URL, NULL, NULL);
	
}

bool UAuthenticator::ListenerCallback(FSocket* socket, const FIPv4Endpoint& endpoint)
{
	uint32 payloadSize;
	TArray<uint8> buffer;
	FString result;
	UE_LOG(LogTemp, Display, TEXT("[Listen Server Callback]"));

	if (socket->HasPendingData(payloadSize))
	{
		UE_LOG(LogTemp, Display, TEXT("[Listen Server Callback has data]"));
		if (payloadSize < 8192 && payloadSize > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Listen Server Callback has valid data]"));
			buffer.SetNumUninitialized(payloadSize);
			int32 read = 0;
			socket->Recv(buffer.GetData(), buffer.Num(), read);
			result = BytesToString(buffer.GetData(), buffer.Num());
			UE_LOG(LogTemp, Display, TEXT("Response: %s"), *result);
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Listen Server Callback invalid data]"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Listen Server Callback has NO data]"));
	}
	return false;
}

void UAuthenticator::StartListener()
{
	FIPv4Address address;
	address.A = 127;
	address.B = 0;
	address.C = 0;
	address.D = 1;

	//is this platform specific it appears to be in the Unity project
	FIPv4Endpoint endpoint(address, this->WINDOWS_IPC_PORT);
	FTcpListener * m_listener = new FTcpListener(endpoint);
	m_listener->OnConnectionAccepted().BindUObject(this, &UAuthenticator::ListenerCallback);
	//																																																																																																																																																																																																																																																																									72934F1640986E3F5CE807AECE930A92
	//“sdk-powered-by-sequence:/oauth2callback#?id_token=eyJhbGciOiJSUzI1NiIsImtpZCI6IjBlNzJkYTFkZjUwMWNhNmY3NTZiZjEwM2ZkN2M3MjAyOTQ3NzI1MDYiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJhY2NvdW50cy5nb29nbGUuY29tIiwiYXpwIjoiOTcwOTg3NzU2NjYwLTM1YTZ0YzQ4aHZpOGNldjljbmtucDBpdWd2OXBvYTIzLmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwiYXVkIjoiOTcwOTg3NzU2NjYwLTM1YTZ0YzQ4aHZpOGNldjljbmtucDBpdWd2OXBvYTIzLmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwic3ViIjoiMTA2NDM4MDU4ODI3OTY4ODY4MDU4IiwiZW1haWwiOiJqMzcyNzQ0NUBnbWFpbC5jb20iLCJlbWFpbF92ZXJpZmllZCI6dHJ1ZSwibm9uY2UiOiI4NEUzMTlGQjQwN0ExODZGNzdBNjRBQkRFRTU0RUY4MyIsIm5iZiI6MTcwMTI4ODE5NiwiaWF0IjoxNzAxMjg4NDk2LCJleHAiOjE3MDEyOTIwOTYsImp0aSI6ImRlZWRhNDJhNWJiMzgyNTBkODkxMDU2NDU2OGM2Yzc1ZmE0ZjJlMjEifQ.p6BBfwApWASellf36uRqfK09ugYFhMWWzypMFvxho-jOQs5ifE3LACfg5qWWnHPR5ADENCGRBRkmVmTvT9y6s8-P_h3TF6P5mLJRLuSWsNRgzqmCWPaysQPuIKpZBh_CoWqGPfWbEuVzLzLrUiMsvJbpsKITR8w80qLGxg2nDyvsyLQ5PPFAmq6j4Q6I_OV7P2Z61dLqxfC-raawxzmrGyiTzxVlxlJ6qyScasPoVL2Q2epCqcX1H2zlOlcw8FG8nMsPP7JxVouIrc6D7KguKV6l4A-hgZVE5I42FHifUKvhPR3mA_JoGgoXXwBaM7mEhc0Thq3Izyy1M80Lli6ang&state=72934F1640986E3F5CE807AECE930A92” 
	//https://accounts.google.com/o/oauth2/auth?response_type=id_token&client_id=970987756660-35a6tc48hvi8cev9cnknp0iugv9poa23.apps.googleusercontent.com&redirect_uri=https://3d41-142-115-54-118.ngrok-free.app/&scope=openid+profile+email&state=72934F1640986E3F5CE807AECE930A92&nonce=84E319FB407A186F77A64ABDEE54EF83

	//Everything I see at this level points to this TCP listen server working and listening despite receiving no traffic yet.
	//TODO what's a better grasp of where our TokenID will be coming from ensure that we are able to get that data!
	if (m_listener->Init())
	{
		UE_LOG(LogTemp, Display, TEXT("[Init listener server]: Listener Active: %s"),(m_listener->IsActive()? TEXT("True") : TEXT("False")));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Failed to Init listener server]"));
	}
}