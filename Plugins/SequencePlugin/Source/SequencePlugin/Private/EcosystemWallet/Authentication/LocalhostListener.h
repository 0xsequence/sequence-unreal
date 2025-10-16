// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpResultCallback.h"
#include "HttpRouteHandle.h"
#include "IHttpRouter.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "LocalhostListener.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API ULocalhostListener : public UObject
{
	GENERATED_BODY()

public:
	static void ClearInstance();
	static ULocalhostListener* GetInstance();
	
	void WaitForResponse(TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);
	void StopListening();

private:
	bool HandleAnyRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

private:
	static ULocalhostListener* Instance;
	
	TSharedPtr<IHttpRouter> Router;
	TOptional<FHttpRouteHandle> RouteHandle;
	uint32 Port = 4444;
	uint32 FallbackPort = 4445;
	bool bServerStarted = false;

	TSuccessCallback<FString> CurrentOnSuccess;
	FFailureCallback CurrentOnFailure;
};
