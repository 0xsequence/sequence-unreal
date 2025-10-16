#pragma once

#include "EcosystemWallet/Guard/Models/GuardConfig.h"
#include "EcosystemWallet/Primitives/Attestation/Attestation.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"
#include "EcosystemWallet/Signers/SessionCredentialsSerializer.h"
#include "SessionCredentials.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FSessionCredentials
{
	GENERATED_USTRUCT_BODY()

	bool IsExplicit;
	FString PrivateKey;
	FString SessionAddress;
	
	FString WalletAddress;
	FString LoginMethod;
	FString UserEmail;
	FGuardConfig Guard;
	FAttestation Attestation;
	FRSY Signature;

	inline static bool CustomJson = true;

	TSharedPtr<FJsonObject> ToJson()
	{
		TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();

		Root->SetBoolField(TEXT("isExplicit"), IsExplicit);
		Root->SetStringField(TEXT("privateKey"), PrivateKey);
		Root->SetStringField(TEXT("sessionAddress"), SessionAddress);
		Root->SetStringField(TEXT("walletAddress"), WalletAddress);
		Root->SetStringField(TEXT("userEmail"), UserEmail);
		Root->SetStringField(TEXT("loginMethod"), LoginMethod);

		Root->SetObjectField(TEXT("guard"), FSessionCredentialsSerializer::MakeGuard(Guard));
		Root->SetObjectField(TEXT("attestation"), FSessionCredentialsSerializer::MakeAttestation(Attestation));
		Root->SetObjectField(TEXT("signature"), FSessionCredentialsSerializer::MakeSignature(Signature));
		
		return Root;
	}

	static FSessionCredentials FromJson(const TSharedPtr<FJsonObject>& Json)
	{
		FGuardConfig GuardConfig = FGuardConfig();
		FAttestation Attestation = FAttestation();
		FRSY Signature = FRSY();

		bool IsExplicit = false;
		if (Json->HasField(TEXT("isExplicit")))
		{
			IsExplicit = Json->GetBoolField(TEXT("isExplicit"));
		}

		FString PrivateKey = "";
		if (Json->HasField(TEXT("privateKey")))
		{
			PrivateKey = Json->GetStringField(TEXT("privateKey"));
		}

		FString SessionAddress = "";
		if (Json->HasField(TEXT("sessionAddress")))
		{
			SessionAddress = Json->GetStringField(TEXT("sessionAddress"));
		}
		
		FString WalletAddress = Json->GetStringField(TEXT("walletAddress"));
		FString UserEmail = Json->GetStringField(TEXT("userEmail"));
		FString LoginMethod = Json->GetStringField(TEXT("loginMethod"));

		FSessionCredentialsSerializer::ParseGuard(Json->GetObjectField(TEXT("guard")), GuardConfig);
		FSessionCredentialsSerializer::ParseAttestation(Json->GetObjectField(TEXT("attestation")), Attestation);
		FSessionCredentialsSerializer::ParseSignature(Json->GetObjectField(TEXT("signature")), Signature);

		return FSessionCredentials {
			IsExplicit,
			PrivateKey,
			SessionAddress,
			WalletAddress,
			LoginMethod,
			UserEmail,
			GuardConfig,
			Attestation,
			Signature
		};
	}
};
