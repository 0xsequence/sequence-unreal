#pragma once
#include "AttestationAuthData.h"
#include "Util/ByteArrayUtils.h"
#include "Util/SequenceCoder.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"

class SEQUENCEPLUGIN_API FAttestation
{
public:
	FAttestation() :
		ApprovedSigner(""),
		IdentityType(TArray<uint8>()),
		IssuerHash(TArray<uint8>()),
		AudienceHash(TArray<uint8>()),
		ApplicationData(TArray<uint8>()),
		AuthData(FAttestationAuthData()) { }
	
	TArray<uint8> Encode()
	{
		return FByteArrayUtils::ConcatBytes({
			FByteArrayUtils::PadLeft(FByteArrayUtils::HexStringToBytes(ApprovedSigner), 20),
			FByteArrayUtils::SliceBytes(IdentityType, 0, 4),
			FByteArrayUtils::PadLeft(IssuerHash, 32),
			FByteArrayUtils::PadLeft(AudienceHash, 32),
			FByteArrayUtils::ByteArrayFromNumber(ApplicationData.Num(), 3),
			ApplicationData,
			AuthData.Encode()
		});
	}

	TArray<uint8> Hash()
	{
		return FSequenceCoder::KeccakHash(Encode());
	}

	FString ToJsonString() const
	{
		const TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();

		RootObject->SetStringField(TEXT("approvedSigner"), ApprovedSigner);
		RootObject->SetStringField(TEXT("identityType"), FByteArrayUtils::BytesToHexString(IdentityType));
		RootObject->SetStringField(TEXT("issuerHash"), FByteArrayUtils::BytesToHexString(IssuerHash));
		RootObject->SetStringField(TEXT("audienceHash"), FByteArrayUtils::BytesToHexString(AudienceHash));
		RootObject->SetStringField(TEXT("applicationData"), FByteArrayUtils::BytesToHexString(ApplicationData));

		const TSharedPtr<FJsonObject> AuthDataObj = MakeShared<FJsonObject>();
		AuthDataObj->SetStringField(TEXT("redirectUrl"), AuthData.RedirectUrl);
		AuthDataObj->SetStringField(TEXT("issuedAt"), AuthData.IssuedAt.Value);

		RootObject->SetObjectField(TEXT("authData"), AuthDataObj);

		FString OutputString;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

		return OutputString;
	}

	FString ApprovedSigner;
	TArray<uint8> IdentityType;
	TArray<uint8> IssuerHash;
	TArray<uint8> AudienceHash;
	TArray<uint8> ApplicationData;
	FAttestationAuthData AuthData;
};
