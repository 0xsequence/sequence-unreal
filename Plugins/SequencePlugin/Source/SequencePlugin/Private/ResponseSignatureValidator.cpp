#include "ResponseSignatureValidator.h"
#include "Misc/Base64.h"
#include "Bitcoin-Cryptography-Library/Cpp/Sha256.hpp"
#include "Bitcoin-Cryptography-Library/Cpp/Sha256Hash.hpp"

bool UResponseSignatureValidator::ValidateResponseSignature(const FHttpResponsePtr& Response)
{
    if (!Response.IsValid())
        return false; 

    const FString ContentDigestHeader = Response->GetHeader(TEXT("content-digest"));
    const FString SignatureHeader = Response->GetHeader(TEXT("signature"));
    const FString SignatureInputHeader = Response->GetHeader(TEXT("signature-input"));

    if (ContentDigestHeader.IsEmpty() || SignatureHeader.IsEmpty() || SignatureInputHeader.IsEmpty())
        return false;

    TArray<uint8> ResponseContent = Response->GetContent();
    FString CleanContentDigestHeader = ContentDigestHeader;
    const FString Prefix = TEXT("sha-256=");
    if (CleanContentDigestHeader.StartsWith(Prefix))
        CleanContentDigestHeader = CleanContentDigestHeader.RightChop(Prefix.Len());

    CleanContentDigestHeader.RemoveAt(0);
    CleanContentDigestHeader.RemoveAt(CleanContentDigestHeader.Len() - 1);

    TArray<uint8_t> DecodedDigest;

    if (!FBase64::Decode(CleanContentDigestHeader, DecodedDigest))
        return false;   

    Sha256Hash ComputedHash = Sha256().getHash(ResponseContent.GetData(), ResponseContent.Num());

    TArray<uint8_t> HashBytes = TArray<uint8_t>(ComputedHash.value, ComputedHash.HASH_LEN);
    FString ExpectedDigestBase64 = FBase64::Encode(HashBytes);

    if (ExpectedDigestBase64 != CleanContentDigestHeader)
    {
        UE_LOG(LogTemp, Error, TEXT("Content-Digest header does not match response content: %s != %s"), *CleanContentDigestHeader, *ExpectedDigestBase64);  
        return false; 
    }

    return true;
}