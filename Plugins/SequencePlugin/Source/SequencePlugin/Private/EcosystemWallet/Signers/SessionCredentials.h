#pragma once

#include "EcosystemWallet/Guard/Models/GuardConfig.h"
#include "EcosystemWallet/Primitives/Attestation/Attestation.h"
#include "EcosystemWallet/Primitives/Attestation/AttestationAuthData.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"
#include "EthAbi/EthAbiBridge.h"
#include "Util/ByteArrayUtils.h"
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

		Root->SetObjectField(TEXT("guard"), MakeGuard(Guard));
		Root->SetObjectField(TEXT("attestation"), MakeAttestation(Attestation));
		Root->SetObjectField(TEXT("signature"), MakeSignature(Signature));
		
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

		ParseGuard(Json->GetObjectField(TEXT("guard")), GuardConfig);
		ParseAttestation(Json->GetObjectField(TEXT("attestation")), Attestation);
		ParseSignature(Json->GetObjectField(TEXT("signature")), Signature);

		UE_LOG(LogTemp, Display, TEXT("PrivateKey: %s"), *PrivateKey);
		UE_LOG(LogTemp, Display, TEXT("SessionAddress: %s"), *SessionAddress);
		UE_LOG(LogTemp, Display, TEXT("WalletAddress: %s"), *WalletAddress);
		UE_LOG(LogTemp, Display, TEXT("UserEmail: %s"), *UserEmail);
		
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

private:

	// Deserialization

	static bool ParseGuard(const TSharedPtr<FJsonObject>& Obj, FGuardConfig& Out)
	{
		if (!Obj.IsValid())
		{
			return false;
		}
		
		Out.Url = Obj->GetStringField(TEXT("url"));

		const TSharedPtr<FJsonObject>* ModuleObj;
		if (Obj->TryGetObjectField(TEXT("moduleAddresses"), ModuleObj))
		{
			const TArray<TSharedPtr<FJsonValue>>* DataArray;
			if ((*ModuleObj)->TryGetArrayField(TEXT("data"), DataArray))
			{
				for (const auto& PairValue : *DataArray)
				{
					const TArray<TSharedPtr<FJsonValue>>* Pair;
					if (PairValue->TryGetArray(Pair) && Pair->Num() == 2)
					{
						FString Key = (*Pair)[0]->AsString();
						FString Value = (*Pair)[1]->AsString();
						Out.ModuleAddresses.Add(Key, Value);
					}
				}
			}
		}
		
		return true;
	}

	static bool ParseSignature(const TSharedPtr<FJsonObject>& Obj, FRSY& Out)
	{
		if (!Obj.IsValid())
		{
			return false;
		}
		
		ParseBigInt(Obj->GetObjectField(TEXT("r")), Out.R);
		ParseBigInt(Obj->GetObjectField(TEXT("s")), Out.S);
		Out.YParity = Obj->GetIntegerField(TEXT("yParity"));
		
		return true;
	}
	
	static bool ParseAttestation(const TSharedPtr<FJsonObject>& Obj, FAttestation& Out)
	{
		if (!Obj.IsValid())
		{
			return false;
		}

		Out.ApprovedSigner = Obj->GetStringField(TEXT("approvedSigner"));
		ParseUint8Array(Obj->GetObjectField(TEXT("identityType")), Out.IdentityType);
		ParseUint8Array(Obj->GetObjectField(TEXT("issuerHash")), Out.IssuerHash);
		ParseUint8Array(Obj->GetObjectField(TEXT("audienceHash")), Out.AudienceHash);
		ParseUint8Array(Obj->GetObjectField(TEXT("applicationData")), Out.ApplicationData);

		const TSharedPtr<FJsonObject>* AuthObj;
		if (Obj->TryGetObjectField(TEXT("authData"), AuthObj))
		{
			ParseAuthData(*AuthObj, Out.AuthData);
		}
		
		return true;
	}
	
	static bool ParseAuthData(const TSharedPtr<FJsonObject>& Obj, FAttestationAuthData& Out)
	{
		if (!Obj.IsValid())
		{
			return false;
		}

		Out.RedirectUrl = Obj->GetStringField(TEXT("redirectUrl"));

		const TSharedPtr<FJsonObject>* IssuedAtObj;
		if (Obj->TryGetObjectField(TEXT("issuedAt"), IssuedAtObj))
		{
			ParseBigInt(*IssuedAtObj, Out.IssuedAt);
		}
		
		return true;
	}

	static bool ParseBigInt(const TSharedPtr<FJsonObject>& Obj, FBigInt& Out)
	{
		if (!Obj.IsValid())
		{
			return false;
		}
		
		Out = FBigInt::FromHex(Obj->GetStringField(TEXT("data")));
		return true;
	}

	static bool ParseUint8Array(const TSharedPtr<FJsonObject>& Obj, TArray<uint8>& Out)
	{
		if (!Obj.IsValid())
		{
			return false;
		}

		Out = FByteArrayUtils::HexStringToBytes(Obj->GetStringField(TEXT("data")));
		return true;
	}

	// Serialization

	TSharedPtr<FJsonObject> MakeGuard(const FGuardConfig& In)
	{
		TSharedPtr<FJsonObject> GuardObj = MakeShared<FJsonObject>();
		GuardObj->SetStringField(TEXT("url"), In.Url);

		TSharedPtr<FJsonObject> ModuleObj = MakeShared<FJsonObject>();
		ModuleObj->SetBoolField(TEXT("_isMap"), true);

		TArray<TSharedPtr<FJsonValue>> DataArray;
		for (const auto& Pair : In.ModuleAddresses)
		{
			TArray<TSharedPtr<FJsonValue>> InnerPair;
			InnerPair.Add(MakeShared<FJsonValueString>(Pair.Key));
			InnerPair.Add(MakeShared<FJsonValueString>(Pair.Value));
			DataArray.Add(MakeShared<FJsonValueArray>(InnerPair));
		}
		ModuleObj->SetArrayField(TEXT("data"), DataArray);
		GuardObj->SetObjectField(TEXT("moduleAddresses"), ModuleObj);

		return GuardObj;
	}

	TSharedPtr<FJsonObject> MakeAuthData(const FAttestationAuthData& In)
	{
		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetStringField(TEXT("redirectUrl"), In.RedirectUrl);
		Obj->SetObjectField(TEXT("issuedAt"), MakeBigInt(In.IssuedAt));
		return Obj;
	}

	TSharedPtr<FJsonObject> MakeAttestation(const FAttestation& In)
	{
		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetStringField(TEXT("approvedSigner"), In.ApprovedSigner);
		Obj->SetObjectField(TEXT("identityType"), MakeUint8Array(In.IdentityType));
		Obj->SetObjectField(TEXT("issuerHash"), MakeUint8Array(In.IssuerHash));
		Obj->SetObjectField(TEXT("audienceHash"), MakeUint8Array(In.AudienceHash));
		Obj->SetObjectField(TEXT("applicationData"), MakeUint8Array(In.ApplicationData));
		Obj->SetObjectField(TEXT("authData"), MakeAuthData(In.AuthData));
		return Obj;
	}

	TSharedPtr<FJsonObject> MakeSignature(const FRSY& In)
	{
		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetObjectField(TEXT("r"), MakeBigInt(In.R));
		Obj->SetObjectField(TEXT("s"), MakeBigInt(In.S));
		Obj->SetNumberField(TEXT("yParity"), In.YParity);
		return Obj;
	}
	
	TSharedPtr<FJsonObject> MakeBigInt(const FBigInt& In)
	{
		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetBoolField(TEXT("_isBigInt"), true);
		Obj->SetStringField(TEXT("data"), FByteArrayUtils::BytesToHexString(FEthAbiBridge::EncodeBigInteger(In.Value)));
		return Obj;
	}

	TSharedPtr<FJsonObject> MakeUint8Array(const TArray<uint8>& In)
	{
		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetBoolField(TEXT("_isUint8Array"), true);
		Obj->SetStringField(TEXT("data"), FByteArrayUtils::BytesToHexString(In));
		return Obj;
	}
};
