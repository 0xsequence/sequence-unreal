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
	void WaitForResponse(TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

private:
	bool HandleAnyRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

private:
	TSharedPtr<IHttpRouter> Router;
	TOptional<FHttpRouteHandle> RouteHandle;
	uint32 Port = 4444;
	bool bServerStarted = false;

	TSuccessCallback<FString> CurrentOnSuccess;
	FFailureCallback CurrentOnFailure;
};
