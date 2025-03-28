// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"
#include "Math/BigInt.h"
#include "DelayedEncodingArgsBP.generated.h"

class UDelayedEncodingObjectArgsBP;
class UDelayedEncodingArrayArgsBP;

UCLASS(BlueprintType, Abstract)
class SEQUENCEPLUGIN_API UDelayedEncodingArgsBP : public UObject
{
	GENERATED_BODY()
public:
	virtual FString GetJsonString();
};

UCLASS(BlueprintType)
class SEQUENCEPLUGIN_API UDelayedEncodingArrayArgsBP : public UDelayedEncodingArgsBP
{
	GENERATED_BODY()
private:
	TArray<TSharedPtr<FJsonValue>> JsonArrayArg;
public:

	/*
	* Used to add a string Arg, Note: adding " inside of strings provided will automatically be given a leading \,
	* to ensure json compatibility.
	*/
	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddStringArg(const FString& ArgIn);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddBoolArg(const bool ArgIn);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddFloatArg(const float ArgIn);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddDoubleArg(const double ArgIn);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddInt32Arg(const int32 ArgIn);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddInt64Arg(const int64 ArgIn);

	UFUNCTION(BlueprintCallable, Category = "Delayed Encoding")
	void AddArrayArg(UDelayedEncodingArrayArgsBP * ArgIn);

	virtual FString GetJsonString() override;

	TArray<TSharedPtr<FJsonValue>> GetJson();
};