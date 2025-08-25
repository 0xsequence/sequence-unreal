#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformProcess.h"
#include "Misc/Base64.h"
#include "IRedirectHandler.h"

class ULocalhostServer;

class SEQUENCEPLUGIN_API FLocalhostRedirectHandler final : public IRedirectHandler
{
protected:
	virtual TTuple<bool, FString> WaitForResponseImpl(const FString& FullUrl) override;
};