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
        const TArray<TSharedPtr<FJsonValue>> Keys = JsonObject->GetArrayField("keys");
        const TSharedPtr<FJsonObject> Key = Keys[0]->AsObject();

        FString ExponentStr = Key->GetStringField("e");
        FString ModulusStr = Key->GetStringField("n");

        TArray<uint8> ExponentBytes = Base64UrlDecode(ExponentStr);
        TArray<uint8> ModulusBytes = Base64UrlDecode(ModulusStr);

        BIGNUM* ExponentBN = BN_bin2bn(ExponentBytes.GetData(), ExponentBytes.Num(), NULL);
        BIGNUM* ModulusBN = BN_bin2bn(ModulusBytes.GetData(), ModulusBytes.Num(), NULL);

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
    return FString(); 
}

bool UResponseSignatureValidator::ValidateResponseSignature(const FHttpResponsePtr& Response)
{
    if (!Response.IsValid())
        return false; 

    const FString ContentDigestHeader = Response->GetHeader(TEXT("content-digest"));
    const FString SignatureHeader = Response->GetHeader(TEXT("signature"));
    const FString SignatureInputHeader = Response->GetHeader(TEXT("signature-input"));

    if (ContentDigestHeader.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("ContentDigestHeader is empty."));
        return false;
    }

    if (SignatureHeader.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("SignatureHeader is empty."));
        return false;
    }

    if (SignatureInputHeader.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("SignatureInputHeader is empty."));
        return false;
    }

    TArray<uint8> ResponseContent = Response->GetContent();
    FString CleanContentDigestHeader = ContentDigestHeader;
    const FString Prefix = TEXT("sha-256=");
    if (CleanContentDigestHeader.StartsWith(Prefix))
        CleanContentDigestHeader = CleanContentDigestHeader.RightChop(Prefix.Len());

    CleanContentDigestHeader.RemoveAt(0);
    CleanContentDigestHeader.RemoveAt(CleanContentDigestHeader.Len() - 1);

    TArray<uint8_t> DecodedDigest;

    if (!FBase64::Decode(CleanContentDigestHeader, DecodedDigest))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to decode Base64 content digest: %s"), *CleanContentDigestHeader);
        return false;
    }

    Sha256Hash ComputedHash = Sha256().getHash(ResponseContent.GetData(), ResponseContent.Num());

    TArray<uint8_t> HashBytes = TArray<uint8_t>(ComputedHash.value, ComputedHash.HASH_LEN);
    FString ExpectedDigestBase64 = FBase64::Encode(HashBytes);

    if (ExpectedDigestBase64 != CleanContentDigestHeader)
    {
        UE_LOG(LogTemp, Error, TEXT("Content-Digest header does not match response content: %s != %s"), *CleanContentDigestHeader, *ExpectedDigestBase64);  
        return false; 
    }

    FString Signature = GetValue(SignatureHeader);
    if (Signature.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse Signature header."));
        return false;
    }

    FString SignatureInputValue = SignatureInputHeader.Replace(TEXT("sig="), TEXT(""));

    FString SignatureBase = FString::Printf(TEXT("\"content-digest\": sha-256=:%s:\n\"@signature-params\": %s"),
        *CleanContentDigestHeader, *SignatureInputValue);

    UE_LOG(LogTemp, Log, TEXT("Signature Base: %s"), *SignatureBase);

    return VerifySignature(SignatureBase, Signature);
}

bool UResponseSignatureValidator::VerifySignature(const FString& SignatureBase, const FString& Signature)
{
    TArray<uint8> SignatureBytes;
    if (!FBase64::Decode(Signature, SignatureBytes))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to decode signature from Base64"));
        return false;  
    }

    TArray<uint8> DataBytes;
    FTCHARToUTF8 Converter(*SignatureBase);
    DataBytes.Append((uint8*)Converter.Get(), Converter.Length());

    EVP_PKEY* PublicKey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(PublicKey, WaasPublicKey); 

    EVP_MD_CTX* mdCtx = EVP_MD_CTX_new();
    EVP_PKEY_CTX* pkeyCtx = nullptr;

    if (EVP_DigestVerifyInit(mdCtx, &pkeyCtx, EVP_sha256(), nullptr, PublicKey) <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize signature verification"));
        EVP_MD_CTX_free(mdCtx);
        EVP_PKEY_free(PublicKey);
        return false;  
    }

    if (EVP_DigestVerifyUpdate(mdCtx, DataBytes.GetData(), DataBytes.Num()) <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to update signature verification with data"));
        EVP_MD_CTX_free(mdCtx);
        EVP_PKEY_free(PublicKey);
        return false;  
    }

    int result = EVP_DigestVerifyFinal(mdCtx, SignatureBytes.GetData(), SignatureBytes.Num());

    EVP_MD_CTX_free(mdCtx);
    EVP_PKEY_free(PublicKey);

    if (result != 1)
    {
        UE_LOG(LogTemp, Error, TEXT("Signature is invalid"));
        return false; 
    }

    UE_LOG(LogTemp, Log, TEXT("Signature is valid"));
    return true; 
}