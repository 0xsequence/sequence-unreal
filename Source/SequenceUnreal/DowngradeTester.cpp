// Fill out your copyright notice in the Description page of Project Settings.


#include "DowngradeTester.h"
#include "SequenceAuthenticator.h"


// Sets default values
ADowngradeTester::ADowngradeTester()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADowngradeTester::BeginPlay()
{
	Super::BeginPlay();
	Authenticate();
}

// Called every frame
void ADowngradeTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADowngradeTester::Authenticate()
{
	USequenceAuthenticator * Authenticator = NewObject<USequenceAuthenticator>();
	const FStoredCredentials_BE Credentials = Authenticator->GetStoredCredentials();
	if (!Credentials.GetValid())
	{
		Authenticator->GuestLogin(false);
	}
}

