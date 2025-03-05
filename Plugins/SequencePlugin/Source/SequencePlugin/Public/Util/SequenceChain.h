#pragma once

#include "CoreMinimal.h"

class SequenceChain
{
public:
	static void SetChainId(int64 NewChainId);
	
	static int64 GetChainId();
	static FString GetChainName();
private:
	static int64 ChainId;
};
