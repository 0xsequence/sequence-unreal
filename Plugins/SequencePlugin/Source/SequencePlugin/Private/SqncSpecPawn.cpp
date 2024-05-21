// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "SqncSpecPawn.h"
#include "Indexer/IndexerSupport.h"
#include "Sequence/SequenceAPI.h"

void ASqncSpecPawn::SetupCredentials(FCredentials_BE CredentialsIn)
{
	this->Credentials = CredentialsIn;
	const FString CredentialsParsed = UIndexerSupport::StructToString(CredentialsIn);
}