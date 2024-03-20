#pragma once
#include "SequencePlugin/Public/Authenticator.h"

inline UAuthenticator * AuthBridgeCallback;
//create void(*Callback) here process to FString then call AuthCallback
void GetIdToken(const FString& providerUrl, void (*Callback)(char *idToken));
void InitiateIosSSO(const FString& providerUrl, UAuthenticator * Callback);
