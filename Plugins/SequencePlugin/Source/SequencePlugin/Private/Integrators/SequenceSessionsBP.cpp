// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Integrators/SequenceSessionsBP.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Util/Log.h"

USequenceSessionsBP* USequenceSessionsBP::GetSubSystem()
{
	if (GEngine)
	{
		const TIndirectArray<FWorldContext> Contexts = GEngine->GetWorldContexts();
		for (FWorldContext Context : Contexts)
		{
			if (const UWorld* World = Context.World())
			{
				if (const UGameInstance* GI = UGameplayStatics::GetGameInstance(World))
				{
					if (USequenceSessionsBP* Subsystem = GI->GetSubsystem<USequenceSessionsBP>())
					{
						return Subsystem;
					}
				}
			}
		}
	}
	else
	{
		SEQ_LOG(Error,TEXT("Error Accessing GEngine"));
	}
	
	SEQ_LOG(Error,TEXT("Error Accessing USequenceWallet GameInstanceSubSystem"));
	return nullptr;
}

USequenceSessionsBP::USequenceSessions()
{
	this->RPCManager = USequenceRPCManager::Make(false);
}

void USequenceSessionsBP::StartEmailLoginAsync(const FString& Email)
{
	const TFunction<void()> OnSuccess = [this]
	{
		this->EmailLoginRequiresCode.Broadcast();
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Email Auth Error: %s"), *Error.Message);
		this->AuthFailed.Broadcast(Error.Message);
	};
	
	this->RPCManager->InitEmailAuth(Email.ToLower(),OnSuccess,OnFailure);
}

void USequenceSessionsBP::ConfirmEmailLoginWithCodeAsync(const FString& Code)
{
	const TSuccessCallback<FCredentials_BE> OnSuccess = [this](const FCredentials_BE& Credentials)
	{
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Email Auth Error: %s"), *Error.Message);
		this->AuthFailed.Broadcast(Error.Message);
	};

	const TFunction<void (FFederationSupportData)> OnFederationRequired = [this](const FFederationSupportData& FederationData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Account Force Create Or Federation Required"));
	};
	
	this->RPCManager->OpenEmailSession(Code, true, OnSuccess, OnFailure, OnFederationRequired);
}
