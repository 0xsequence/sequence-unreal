#pragma once

#include "UObject/Interface.h"
#include "INativeAuthCallback.generated.h"

UINTERFACE()
class UNativeAuthCallback : public UInterface
{
	GENERATED_BODY()
};

class INativeAuthCallback
{
	GENERATED_BODY()

public:
	virtual void HandleNativeIdToken(const FString& IdToken); 
	virtual void HandleNativeTokenizedUrl(const FString& TokenizedUrl); 
};
