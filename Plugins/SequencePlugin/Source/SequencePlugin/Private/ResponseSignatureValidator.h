#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "http.h"
#include "ResponseSignatureValidator.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UResponseSignatureValidator : public UObject
{
    GENERATED_BODY()

public:
    // Public method to validate the signature from an FHttpResponsePtr
    
    bool ValidateResponseSignature(const FHttpResponsePtr& Response);

private:

   
};

//UResponseSignatureValidator