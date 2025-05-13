#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Http.h"

#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include "openssl/rsa.h"
THIRD_PARTY_INCLUDES_END
#undef UI


#include "ResponseSignatureValidator.generated.h"



UCLASS()
class SEQUENCEPLUGIN_API UResponseSignatureValidator : public UObject
{
    GENERATED_BODY()

public:
    // Public method to validate the signature from an FHttpResponsePtr
    
    UResponseSignatureValidator();

    bool PublicKeyFetched;

    bool ValidateResponseSignature(const FHttpResponsePtr& Response);

    bool VerifySignature(const FString& SignatureBase, const FString& Signature);

    bool HasFoundTamperedResponse();

private:

    bool tamperedResponseSignatureFound = false;

    RSA* WaasPublicKey;

    const FString JWKS = "{\"keys\":[{\"alg\":\"RS256\",\"e\":\"AQAB\",\"kid\":\"nWh-_3nQ1lnhhI1ZSQTQmw\",\"kty\":\"RSA\",\"n\":\"pECaEq2k0k22J9e7hFLAFmKbzPLlWToUJJmFeWAdEiU4zpW17EUEOyfjRzjgBewc7KFJQEblC3eTD7Vc5bh9-rafPEj8LaKyZzzS5Y9ZATXhlMo5Pnlar3BrTm48XcnT6HnLsvDeJHUVbrYd1JyE1kqeTjUKWvgKX4mgIJiuYhpdzbOC22cPaWb1dYCVhArDVAPHGqaEwRjX7JneETdY5hLJ6JhsAws706W7fwfNKddPQo2mY95S9q8HFxMr5EaXEMmhwxk8nT5k-Ouar2dobMXRMmQiEZSt9fJaGKlK7KWJSnbPOVa2cZud1evs1Rz2SdCSA2bhuZ6NnZCxkqnagw\",\"use\":\"sig\"}]}";


    void LoadPublicKey();

    TArray<uint8> Base64UrlDecode(const FString& Input);

    FString GetValue(const FString& HeaderItem);
   
};
