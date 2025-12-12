#pragma once

#include "CoreMinimal.h"
#include "EcosystemWallet/Guard/Models/SignWith.h"
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
};
