#pragma once

#include "CoreMinimal.h"
#include "Types/BigInt.h"

struct SEQUENCEPLUGIN_API FCall
{
	FString To;
	FString Data;
	FBigInt Value;
	FBigInt GasLimit;
	bool DelegateCall;
	bool OnlyFallback;
	FString BehaviorOnError;

	TArray<uint8> Hash() const;
	
private:
	static constexpr const TCHAR* CallTypeHash = TEXT("Call(bytes32,address,uint256,bytes,uint256,bool,bool,uint8)");
};
