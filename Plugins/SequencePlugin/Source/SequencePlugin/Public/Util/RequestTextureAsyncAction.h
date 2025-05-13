#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "RequestTextureAsyncAction.generated.h"

UCLASS()
class URequestTextureAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTexturePin, UTexture2D*, Texture);
	
public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "0xSequence SDK - Actions")
	static URequestTextureAsyncAction* RequestTextureAsync(const FString& Url);

	virtual void Activate() override;

private:
	FString Url;
	
	UPROPERTY(BlueprintAssignable)
	FTexturePin OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FTexturePin OnFailure;
};
