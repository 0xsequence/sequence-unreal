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
	static FString GetChainIdString();
	static FString GetChainName();
	static FString GetRedirectUrl();
private:
	inline static int64 ChainId = -1;
	inline static FString RedirectUrl = "";
};
