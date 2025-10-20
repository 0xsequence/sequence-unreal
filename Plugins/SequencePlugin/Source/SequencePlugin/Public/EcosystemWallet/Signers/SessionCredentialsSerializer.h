#pragma once
#include "EcosystemWallet/Guard/Models/GuardConfig.h"
#include "EcosystemWallet/Primitives/Attestation/Attestation.h"
#include "EcosystemWallet/Primitives/Attestation/AttestationAuthData.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"

class SEQUENCEPLUGIN_API FSessionCredentialsSerializer
{
public:
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
		
		Out = FBigInt(Obj->GetStringField(TEXT("data")));
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

	static TSharedPtr<FJsonObject> MakeGuard(const FGuardConfig& In)
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

	static TSharedPtr<FJsonObject> MakeAuthData(const FAttestationAuthData& In)
	{
		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetStringField(TEXT("redirectUrl"), In.RedirectUrl);
		Obj->SetObjectField(TEXT("issuedAt"), MakeBigInt(In.IssuedAt));
		return Obj;
	}

	static TSharedPtr<FJsonObject> MakeAttestation(const FAttestation& In)
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

	static TSharedPtr<FJsonObject> MakeSignature(const FRSY& In)
	{
		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetObjectField(TEXT("r"), MakeBigInt(In.R));
		Obj->SetObjectField(TEXT("s"), MakeBigInt(In.S));
		Obj->SetNumberField(TEXT("yParity"), In.YParity);
		return Obj;
	}
	
	static TSharedPtr<FJsonObject> MakeBigInt(const FBigInt& In)
	{
		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetBoolField(TEXT("_isBigInt"), true);
		Obj->SetStringField(TEXT("data"), In.Value);
		return Obj;
	}

	static TSharedPtr<FJsonObject> MakeUint8Array(const TArray<uint8>& In)
	{
		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetBoolField(TEXT("_isUint8Array"), true);
		Obj->SetStringField(TEXT("data"), FByteArrayUtils::BytesToHexString(In));
		return Obj;
	}
};
