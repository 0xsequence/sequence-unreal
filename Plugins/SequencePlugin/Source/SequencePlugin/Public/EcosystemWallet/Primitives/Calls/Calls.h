#pragma once

#include "CoreMinimal.h"
#include "Call.h"
#include "Types/BigInt.h"

struct SEQUENCEPLUGIN_API FCalls
{
	FCalls()
		: Calls(TArray<FCall>()), Space(FBigInt("0")), Nonce(FBigInt("0")) { }
	
	explicit FCalls(const TArray<FCall>& Calls, const FBigInt& Space, const FBigInt& Nonce)
		: Calls(Calls), Space(Space), Nonce(Nonce) { }
	
	TArray<FCall> Calls;
	FBigInt Space;
	FBigInt Nonce;
};
