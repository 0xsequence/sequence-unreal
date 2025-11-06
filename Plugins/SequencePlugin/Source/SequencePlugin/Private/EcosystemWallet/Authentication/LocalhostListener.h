#pragma once

#include "CoreMinimal.h"
#include "HttpResultCallback.h"
#include "HttpRouteHandle.h"
#include "IHttpRouter.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "LocalhostListener.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API ULocalhostListener : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	ULocalhostListener();
	
	static ULocalhostListener* GetInstance();
	
	virtual void BeginDestroy() override
	{
		StopListening();
		Super::BeginDestroy();
	}
	
	void WaitForResponse(TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);
	void StopListening();

	static ULocalhostListener* Instance;

private:
	bool HandleAnyRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	TSharedPtr<IHttpRouter> Router;
	TOptional<FHttpRouteHandle> RouteHandle;
	uint32 Port = 4444;
	uint32 FallbackPort = 4445;
	bool bServerStarted = false;

	TSharedPtr<TSuccessCallback<FString>> StrongOnSuccess;
	TSharedPtr<FFailureCallback> StrongOnFailure;

	TWeakPtr<TSuccessCallback<FString>> CurrentOnSuccess;
	TWeakPtr<FFailureCallback> CurrentOnFailure;
};
