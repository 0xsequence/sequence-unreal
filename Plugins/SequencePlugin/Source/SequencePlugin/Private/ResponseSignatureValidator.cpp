#include "ResponseSignatureValidator.h"
#include "Misc/Base64.h"
#include "Bitcoin-Cryptography-Library/Cpp/Sha256.hpp"
#include "Bitcoin-Cryptography-Library/Cpp/Sha256Hash.hpp"

bool UResponseSignatureValidator::ValidateResponseSignature(const FHttpResponsePtr& Response)
{
    if (!Response.IsValid())
    {
        return false; // Invalid response
    }

    // Extract headers
    const FString ContentDigestHeader = Response->GetHeader(TEXT("content-digest"));
    const FString SignatureHeader = Response->GetHeader(TEXT("signature"));
    const FString SignatureInputHeader = Response->GetHeader(TEXT("signature-input"));

    // Validate that headers are present
    if (ContentDigestHeader.IsEmpty() || SignatureHeader.IsEmpty() || SignatureInputHeader.IsEmpty())
    {
        return false; // Required headers are missing
    }

    // Store the response content as a UTF-8 byte array
    const FString ResponseContent = Response->GetContentAsString();
    const uint8_t* ResponseContentBytes = (const uint8_t*)TCHAR_TO_UTF8(*ResponseContent);
    int32 ResponseContentLength = ResponseContent.Len();

    // Log the raw response content
    UE_LOG(LogTemp, Warning, TEXT("Raw Response Content: %s"), *ResponseContent);
    UE_LOG(LogTemp, Warning, TEXT("Content length: %d"), ResponseContentLength);

    FString CleanContentDigestHeader = ContentDigestHeader;
    const FString Prefix = TEXT("sha-256=");
    if (CleanContentDigestHeader.StartsWith(Prefix))
    {
        CleanContentDigestHeader = CleanContentDigestHeader.RightChop(Prefix.Len());
    }

    CleanContentDigestHeader.RemoveAt(0);
    CleanContentDigestHeader.RemoveAt(CleanContentDigestHeader.Len() - 1);

    // Decode the Content-Digest header from Base64
    TArray<uint8_t> DecodedDigest;
    if (!FBase64::Decode(CleanContentDigestHeader, DecodedDigest))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to decode Content-Digest header: %s"), *CleanContentDigestHeader);
        return false; // Decoding failed
    }

    // Compute the SHA-256 hash of the response content
    Sha256Hash ComputedHash = Sha256().getHash(ResponseContentBytes, ResponseContentLength);

    // Convert the computed hash to a Base64 string
    TArray<uint8_t> HashBytes = TArray<uint8_t>(ComputedHash.value, ComputedHash.HASH_LEN);
    FString ExpectedDigestBase64 = FBase64::Encode(HashBytes);

    // Log the raw Content-Digest header before and after decoding
    UE_LOG(LogTemp, Warning, TEXT("Raw Content-Digest header before decoding: %s"), *CleanContentDigestHeader);
    UE_LOG(LogTemp, Warning, TEXT("Computed Digest: %s"), *ExpectedDigestBase64);

    // Compare the digests
    if (ExpectedDigestBase64 != CleanContentDigestHeader)
    {
        // Log the error if needed
        UE_LOG(LogTemp, Error, TEXT("Content-Digest header does not match response content: %s != %s"),
            *CleanContentDigestHeader, *ExpectedDigestBase64);
        return false; // Or throw an exception if desired
    }

    return true;
}