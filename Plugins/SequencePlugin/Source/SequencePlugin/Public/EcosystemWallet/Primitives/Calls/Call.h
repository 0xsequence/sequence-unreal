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

	TArray<uint8> Encode();
	TArray<uint8> Hash() const;
	
private:
	uint8 GetFlag();
	TArray<uint8> GetAddressBytes(const uint8& Flag);
	TArray<uint8> GetValueBytes(const uint8& Flag);
	TArray<uint8> GetDataBytes(const uint8& Flag);
	TArray<uint8> GetGasLimitBytes(const uint8& Flag);
	
	static constexpr const TCHAR* CallTypeHash = TEXT("Call(address to,uint256 value,bytes data,uint256 gasLimit,bool delegateCall,bool onlyFallback,uint256 behaviorOnError)");
};
