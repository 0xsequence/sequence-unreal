#pragma once
#include "SequencePlugin/Public/Authenticator.h"

inline UAuthenticator * AuthBridgeCallback;
void InitiateIosSSO(const FString& providerUrl, UAuthenticator * Callback);
