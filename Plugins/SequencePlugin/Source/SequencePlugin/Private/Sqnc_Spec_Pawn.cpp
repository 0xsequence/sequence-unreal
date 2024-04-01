// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Sqnc_Spec_Pawn.h"
#include "Indexer/IndexerSupport.h"

void ASqnc_Spec_Pawn::SetupCredentials(FCredentials_BE CredentialsIn)
{
	this->Credentials = CredentialsIn;
	const FString CredentialsParsed = UIndexerSupport::structToString(CredentialsIn);
	UE_LOG(LogTemp,Display,TEXT("Passed Credentials: %s"), *CredentialsParsed);
}