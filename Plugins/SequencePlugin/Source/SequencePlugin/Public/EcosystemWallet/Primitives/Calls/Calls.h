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

	TArray<uint8> Encode();

	TArray<uint8> Hash(const FString& Wallet, const FBigInt& ChainId) const;
	
	TArray<FCall> Calls;
	FBigInt Space;
	FBigInt Nonce;

private:
	uint8 GetFlag();
	uint8 GetCallsCountSize();
	TArray<uint8> GetSpaceBytes();
	TArray<uint8> GetNonceBytes();
	TArray<uint8> GetCallsCount();
	TArray<uint8> GetCallsBytes();
};
