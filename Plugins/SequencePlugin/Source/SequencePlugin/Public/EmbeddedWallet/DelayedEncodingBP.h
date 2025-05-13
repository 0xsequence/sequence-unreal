// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"
#include "DelayedEncodingBP.generated.h"

class UDelayedEncodingArgsBP;

UCLASS(BlueprintType)
class SEQUENCEPLUGIN_API UDelayedEncodingBP : public UObject
{
	GENERATED_BODY()
private:
	FString Abi = "";
	UPROPERTY()
	UDelayedEncodingArgsBP * Args = nullptr;
	FString Func = "";
public:

	/*
	 * Used to set the ABI String, Note you'll need to include \" & \\ where needed
	 * This String will be Enclosed like so: \"Your String\"
	 */
	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void SetAbi(const FString& AbiIn);

	/*
	 * Used to set the Args object
	 */
	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void SetArgs(UDelayedEncodingArgsBP * ArgsIn);

	/*
	 * Used to set the Function string, Note you'll need to include \" & \\ where needed
	 * This String will be Enclosed like so: \"Your String\"
	 */
	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void SetFunc(const FString& FuncIn);

	FString GetJsonString() const;
};