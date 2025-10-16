#include "SessionCallSignature.h"
#include "ExplicitSessionCallSignature.h"
#include "ImplicitSessionCallSignature.h"
#include "Util/ByteArrayUtils.h"

TArray<uint8> FSessionCallSignature::EncodeSignatures(
		const TArray<TSharedPtr<FSessionCallSignature>>& Signatures,
		const TSharedPtr<FSessionsTopology>& SessionsTopology,
		const TArray<FString>& ImplicitSigners,
		const TArray<FString>& ExplicitSigners)
{
	TArray<TArray<uint8>> Parts;

    const FString IdentitySigner = SessionsTopology->GetIdentitySigner();
    const TSharedPtr<FSessionsTopology> MinTopology = SessionsTopology->Minimise(ExplicitSigners, ImplicitSigners, IdentitySigner);

    TArray<uint8> EncodedTopology = MinTopology.Get()->Encode();
    if (FByteArrayUtils::MinBytesFor(EncodedTopology.Num()) > 3)
    {
        UE_LOG(LogTemp, Error, TEXT("Session topology is too large"));
    }

    Parts.Add(FByteArrayUtils::ByteArrayFromNumber(EncodedTopology.Num(), 3));
    Parts.Add(EncodedTopology);

    TMap<FString, int32> AttestationMap;
    TArray<TArray<uint8>> EncodedAttestations;

    for (TSharedPtr<FSessionCallSignature> Signature : Signatures)
    {
        if (Signature->Type != ESessionCallSignatureType::Implicit)
            continue;
        
        auto* ImplicitSignature = static_cast<FImplicitSessionCallSignature*>(Signature.Get());
        if (ImplicitSignature->Attestation != nullptr)
        {
            FString AttestationStr = ImplicitSignature->Attestation.Get()->ToJsonString();
            UE_LOG(LogTemp, Display, TEXT("Adding attestation %s"), *AttestationStr)

            if (!AttestationMap.Contains(AttestationStr))
            {
                int32 Index = EncodedAttestations.Num();
                AttestationMap.Add(AttestationStr, Index);

                TArray<uint8> EncodedAttestation = FByteArrayUtils::ConcatBytes({
                    ImplicitSignature->Attestation->Encode(),
                    ImplicitSignature->IdentitySignature->Pack()
                });

                EncodedAttestations.Add(EncodedAttestation);
            }
        }
    }

    if (EncodedAttestations.Num() >= 128)
    {
        UE_LOG(LogTemp, Error, TEXT("Too many attestations"));
    }

    Parts.Add(FByteArrayUtils::ByteArrayFromNumber(EncodedAttestations.Num(), 1));
    Parts.Add(FByteArrayUtils::ConcatBytes(EncodedAttestations));

    for (TSharedPtr<FSessionCallSignature> Signature : Signatures)
    {
        if (Signature->Type == ESessionCallSignatureType::Implicit)
        {
            const auto* ImplicitCall = static_cast<FImplicitSessionCallSignature*>(Signature.Get());
            
            FString AttestationStr = ImplicitCall->Attestation->ToJsonString();
            if (!AttestationMap.Contains(AttestationStr))
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to find attestation index"));
                continue;
            }
            
            int32 Index = AttestationMap[AttestationStr];

            Parts.Add(FByteArrayUtils::ByteArrayFromNumber(0x80 | Index, 1));
            Parts.Add(ImplicitCall->SessionSignature->Pack());
        }
        else if (Signature->Type == ESessionCallSignatureType::Explicit)
        {
            const auto* ExplicitCall = static_cast<FExplicitSessionCallSignature*>(Signature.Get());
            
            if (ExplicitCall->PermissionIndex > 127)
            {
                UE_LOG(LogTemp, Error, TEXT("Permission index is too large"));
            }

            Parts.Add(FByteArrayUtils::ByteArrayFromNumber(ExplicitCall->PermissionIndex, 1));
            Parts.Add(ExplicitCall->SessionSignature->Pack());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid call signature"));
        }
    }

    return FByteArrayUtils::ConcatBytes(Parts);
}
