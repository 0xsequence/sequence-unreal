// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"
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
class SEQUENCEPLUGIN_API UDelayedEncodingObjectArgsBP : public UDelayedEncodingArgsBP
{
	GENERATED_BODY()
private:
	TSharedPtr<FJsonObject> JsonObjectArg;
public:
	UDelayedEncodingObjectArgsBP();

	/*
	 * Used to add a string Arg, Note: adding " inside of strings provided will automatically be given a leading \,
	 * to ensure json compatibility.
	 */
	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddStringArg(const FString& ArgName, const FString& ArgIn);
	
	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddBoolArg(const FString& ArgName, const bool ArgIn);
	
	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddFloatArg(const FString& ArgName, const float ArgIn);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddDoubleArg(const FString& ArgName, const double ArgIn);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddInt32Arg(const FString& ArgName, const int32 ArgIn);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddInt64Arg(const FString& ArgName, const int64 ArgIn);
	
	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddObjectArg(const FString& ArgName, UDelayedEncodingObjectArgsBP * ArgIn);

	UFUNCTION(BlueprintCallable, Category = "Delayed Encoding")
	void AddArrayArg(const FString& ArgName,UDelayedEncodingArrayArgsBP * ArgIn);

	virtual FString GetJsonString() override;
	
	TSharedPtr<FJsonObject> GetJson();
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
	
	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void AddObjectArg(UDelayedEncodingObjectArgsBP * ArgIn);

	UFUNCTION(BlueprintCallable, Category = "Delayed Encoding")
	void AddArrayArg(UDelayedEncodingArrayArgsBP * ArgIn);

	virtual FString GetJsonString() override;

	TArray<TSharedPtr<FJsonValue>> GetJson();
};