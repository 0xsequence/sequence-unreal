// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"
#include "DelayedEncodingArgsBP.generated.h"

struct FDelayedEncoding;
class UDelayedEncodingBP;

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FDelayedEncodingArgs
{
	GENERATED_USTRUCT_BODY()
private:
	TArray<TSharedPtr<FJsonValue>> Args;
public:
	void AddStringArg(const FString& ArgIn);

	void AddBoolArg(const bool ArgIn);

	void AddFloatArg(const float ArgIn);

	void AddInt32Arg(const int32 ArgIn);

	void AddInt64Arg(const int64 ArgIn);

	void AddDelayedEncodingArg(const FDelayedEncoding& Obj);

	TArray<TSharedPtr<FJsonValue>> GetArgs();
};

UCLASS(BlueprintType)
class SEQUENCEPLUGIN_API UDelayedEncodingArgsBP : public UObject
{
	GENERATED_BODY()
private:
	FDelayedEncodingArgs Args;
public:

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddStringArg(const FString& ArgIn);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddBoolArg(const bool ArgIn);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddFloatArg(const float ArgIn);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddInt32Arg(const int32 ArgIn);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddInt64Arg(const int64 ArgIn);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddDelayedEncodingArg(UDelayedEncodingBP * ArgIn);

	TArray<TSharedPtr<FJsonValue>> GetArgs();
};