#include "ResponseSignatureValidator.h"
#include "Misc/Base64.h"
#include "Json.h"
#include "Bitcoin-Cryptography-Library/Cpp/Sha256.hpp"
#include "Bitcoin-Cryptography-Library/Cpp/Sha256Hash.hpp"
#include "OpenSSL/evp.h"

UResponseSignatureValidator::UResponseSignatureValidator()
{
    LoadPublicKey();
}

void UResponseSignatureValidator::LoadPublicKey()
{
    // Parse the JWKS JSON
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JWKS);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // Extract the key values
        const TArray<TSharedPtr<FJsonValue>> Keys = JsonObject->GetArrayField("keys");
        const TSharedPtr<FJsonObject> Key = Keys[0]->AsObject();

        FString ExponentStr = Key->GetStringField("e");
        FString ModulusStr = Key->GetStringField("n");

        // Decode the Base64 URL values
        TArray<uint8> ExponentBytes = Base64UrlDecode(ExponentStr);
        TArray<uint8> ModulusBytes = Base64UrlDecode(ModulusStr);

        // Convert to OpenSSL BIGNUM format
        BIGNUM* ExponentBN = BN_bin2bn(ExponentBytes.GetData(), ExponentBytes.Num(), NULL);
        BIGNUM* ModulusBN = BN_bin2bn(ModulusBytes.GetData(), ModulusBytes.Num(), NULL);

        // Create the RSA key structure
        WaasPublicKey = RSA_new();
        RSA_set0_key(WaasPublicKey, ModulusBN, ExponentBN, NULL);

        PublicKeyFetched = true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Error loading public key from JWKS"));
        PublicKeyFetched = false;
    }
}

TArray<uint8> UResponseSignatureValidator::Base64UrlDecode(const FString& Input)
{
    FString Output = Input;
    Output = Output.Replace(TEXT("-"), TEXT("+")).Replace(TEXT("_"), TEXT("/"));

    // Adjust for padding
    while (Output.Len() % 4 != 0)
    {
        Output.AppendChar('=');
    }

    TArray<uint8> DecodedBytes;
    FBase64::Decode(Output, DecodedBytes);
    return DecodedBytes;
}

FString UResponseSignatureValidator::GetValue(const FString& HeaderItem)
{
    int32 FirstColon, LastColon;
    if (HeaderItem.FindChar(':', FirstColon) && HeaderItem.FindLastChar(':', LastColon) && FirstColon != LastColon)
    {
        return HeaderItem.Mid(FirstColon + 1, LastColon - FirstColon - 1).TrimStartAndEnd();
    }
    return FString(); // Return empty if no valid value found.
}

bool UResponseSignatureValidator::ValidateResponseSignature(const FHttpResponsePtr& Response)
{
    if (!Response.IsValid())
    {
        tamperedResponseSignatureFound = true;
        return false; // Invalid response
    }

    // Extract headers
    const FString ContentDigestHeader = Response->GetHeader(TEXT("content-digest"));
    const FString SignatureHeader = Response->GetHeader(TEXT("signature"));
    const FString SignatureInputHeader = Response->GetHeader(TEXT("signature-input"));
    
    // Validate that headers are present
    if (ContentDigestHeader.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("ContentDigestHeader is empty."));
        tamperedResponseSignatureFound = true;
        return false;
    }

    if (SignatureHeader.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("SignatureHeader is empty."));
        tamperedResponseSignatureFound = true;
        return false;
    }

    if (SignatureInputHeader.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("SignatureInputHeader is empty."));
        tamperedResponseSignatureFound = true;
        return false;
    }

    TArray<uint8> ResponseContent = Response->GetContent();
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
        UE_LOG(LogTemp, Error, TEXT("Failed to decode Base64 content digest: %s"), *CleanContentDigestHeader);
        tamperedResponseSignatureFound = true;
        return false;
    }

    // Compute the SHA-256 hash of the response content
    Sha256Hash ComputedHash = Sha256().getHash(ResponseContent.GetData(), ResponseContent.Num());

    // Convert the computed hash to a Base64 string
    TArray<uint8_t> HashBytes = TArray<uint8_t>(ComputedHash.value, ComputedHash.HASH_LEN);
    FString ExpectedDigestBase64 = FBase64::Encode(HashBytes);

    if (ExpectedDigestBase64 != CleanContentDigestHeader)
    {
        UE_LOG(LogTemp, Error, TEXT("Content-Digest header does not match response content: %s != %s"), *CleanContentDigestHeader, *ExpectedDigestBase64);
        tamperedResponseSignatureFound = true;
        return false;
    }

    // Construct the signature base
    // Parse the signature value from the header
    FString Signature = GetValue(SignatureHeader);
    if (Signature.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse Signature header."));
        return false;
    }

    // Remove the "sig=" prefix from the SignatureInput header
    FString SignatureInputValue = SignatureInputHeader.Replace(TEXT("sig="), TEXT(""));

    // Construct the SignatureBase string
    FString SignatureBase = FString::Printf(TEXT("\"content-digest\": sha-256=:%s:\n\"@signature-params\": %s"),
        *CleanContentDigestHeader, *SignatureInputValue);

    // Here, you can log or return the SignatureBase if needed
    UE_LOG(LogTemp, Log, TEXT("Signature Base: %s"), *SignatureBase);

    return VerifySignature(SignatureBase, Signature);
}

bool UResponseSignatureValidator::VerifySignature(const FString& SignatureBase, const FString& Signature)
{
    // Convert signature from Base64 to byte array
    TArray<uint8> SignatureBytes;
    if (!FBase64::Decode(Signature, SignatureBytes))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to decode signature from Base64"));
        tamperedResponseSignatureFound = true;
        return false;  // Return false on failure
    }

    // Convert SignatureBase (the data to verify) to byte array (UTF-8 encoding)
    TArray<uint8> DataBytes;
    FTCHARToUTF8 Converter(*SignatureBase);
    DataBytes.Append((uint8*)Converter.Get(), Converter.Length());

    // Verify the signature using the loaded RSA public key
    EVP_PKEY* PublicKey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(PublicKey, WaasPublicKey);  // WaasPublicKey is the RSA key

    EVP_MD_CTX* mdCtx = EVP_MD_CTX_new();
    EVP_PKEY_CTX* pkeyCtx = nullptr;

    // Initialize signature verification
    if (EVP_DigestVerifyInit(mdCtx, &pkeyCtx, EVP_sha256(), nullptr, PublicKey) <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize signature verification"));
        EVP_MD_CTX_free(mdCtx);
        EVP_PKEY_free(PublicKey);
        tamperedResponseSignatureFound = true;
        return false;  // Return false on failure
    }

    // Update with data to verify
    if (EVP_DigestVerifyUpdate(mdCtx, DataBytes.GetData(), DataBytes.Num()) <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to update signature verification with data"));
        EVP_MD_CTX_free(mdCtx);
        EVP_PKEY_free(PublicKey);
        tamperedResponseSignatureFound = true;
        return false;  // Return false on failure
    }

    // Perform the actual verification
    int result = EVP_DigestVerifyFinal(mdCtx, SignatureBytes.GetData(), SignatureBytes.Num());

    // Clean up OpenSSL structures
    EVP_MD_CTX_free(mdCtx);
    EVP_PKEY_free(PublicKey);

    if (result != 1)
    {
        UE_LOG(LogTemp, Error, TEXT("Signature is invalid"));
        tamperedResponseSignatureFound = true;
        return false;  // Return false if signature verification fails
    }

    // If everything was successful
    UE_LOG(LogTemp, Log, TEXT("Signature is valid"));
    return true;  // Return true if verification is successful
}

bool UResponseSignatureValidator::HasFoundTamperedResponse()
{
    return tamperedResponseSignatureFound;
}

//l2am+8M1yDQUrezN2FqVJfMKsngtsv6p9Rvp11Ies0NMYd9lFcoQbqOCdEXklRT4eymVC4iFhUI4I4wimjOTopDptwfyQy8Fyo9tei0CqfnQejdIdIcGDaz6ncn68uow82jQTOuaEjkEHuxnQSRd46KrJQRdvHx7oL9koCxCThISb8JVgiOn7FdS2ZJ4XkU9WpBseL5FLiQW6M3PP9+77WaRJ+5MvVukAva6dblRdx497fB5TUbZsZzMh/duU+M2NkNebCpMmEFgOHsRmMRWSkKHwGhzFycNqiwyxrMOmdludz+Gpi9ZCoaiWuzb1ZC7gvhMCsxWtXc9oG5iv5U3hA==

//"content-digest": sha-256=:g2r98q/9/5HDXq3LaiqNXmHaPQfruwSgefdPzt7uwlg=:"@signature-params": ("content-digest"); created = 1728998376; keyid = "nWh-_3nQ1lnhhI1ZSQTQmw"; alg = "rsa-v1_5-sha256"
