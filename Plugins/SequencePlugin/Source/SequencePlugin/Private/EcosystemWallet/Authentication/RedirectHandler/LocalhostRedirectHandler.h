#pragma once

#include "CoreMinimal.h"
#include "IRedirectHandler.h"
#include "HAL/PlatformProcess.h"
#include "Misc/Base64.h"
#include "Util/Async.h"

class ULocalhostServer;

class SEQUENCEPLUGIN_API FLocalhostRedirectHandler : public IRedirectHandler
{
protected:
	virtual void WaitForResponseImpl(const FString& FullUrl, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure) override;
};