
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
	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	void AddStringArg(const FString& ArgIn);

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	void AddBoolArg(const bool ArgIn);

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	void AddFloatArg(const float ArgIn);

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	void AddDoubleArg(const double ArgIn);

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	void AddInt32Arg(const int32 ArgIn);

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	void AddInt64Arg(const int64 ArgIn);

	UFUNCTION(BlueprintCallable, Category = "Delayed Encoding")
	void AddArrayArg(UDelayedEncodingArrayArgsBP * ArgIn);

	virtual FString GetJsonString() override;

	TArray<TSharedPtr<FJsonValue>> GetJson();
};