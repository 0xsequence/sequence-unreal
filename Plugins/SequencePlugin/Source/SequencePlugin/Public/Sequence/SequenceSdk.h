#pragma once

#include "CoreMinimal.h"

/*
 * Central class for any SDK configuration information.
 */
class SequenceSdk
{
public:
	static void SetChainId(int64 NewChainId);
	static void SetRedirectUrl(const FString& NewRedirectUrl);
	
	static int64 GetChainId();
	static FString GetChainName();
	static FString GetRedirectUrl();
private:
	static int64 ChainId;
	static FString RedirectUrl;
};
