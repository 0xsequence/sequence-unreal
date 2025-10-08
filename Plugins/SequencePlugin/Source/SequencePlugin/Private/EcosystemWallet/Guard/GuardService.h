#pragma once

#include "CoreMinimal.h"
#include "Models/SignWith.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "GuardService.generated.h"

class UHttpHandler;

UCLASS()
class SEQUENCEPLUGIN_API UGuardService : public UObject
{
	GENERATED_BODY()

public:
	void WithHost(const FString& Host);
	
	void SignWith(const FSignWithArgs& Args, TSuccessCallback<FSignWithResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const;
	
private:
	UPROPERTY()
	UHttpHandler* HttpHandler;

	FString SignerAddress = TEXT("0x18002Fc09deF9A47437cc64e270843dE094f5984");
};
