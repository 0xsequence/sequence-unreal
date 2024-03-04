// Fill out your copyright notice in the Description page of Project Settings.

#include "Sqnc_Spec_Pawn.h"
#include "Indexer/IndexerSupport.h"

void ASqnc_Spec_Pawn::SetupCredentials(FCredentials_BE CredentialsIn)
{
	this->Credentials = CredentialsIn;
	const FString CredentialsParsed = UIndexerSupport::structToString(CredentialsIn);
	UE_LOG(LogTemp,Display,TEXT("Passed Credentials: %s"), *CredentialsParsed);
}