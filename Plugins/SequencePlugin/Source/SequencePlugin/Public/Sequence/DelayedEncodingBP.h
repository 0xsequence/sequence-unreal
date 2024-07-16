// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"
#include "DelayedEncodingBP.generated.h"

struct FDelayedEncodingArgs;
class UDelayedEncodingArgsBP;

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FDelayedEncoding
{
	GENERATED_USTRUCT_BODY()
private:
	FString type = "delayedEncode";//Hide this and hard code it
	public:
	//Have this bound to be set either by a custom enum OR by FString
	FString Abi = "[{\"inputs\":[{\"internalType\":\"bytes32\",\"name\":\"_orderId\",\"type\":\"bytes32\"},{\"internalType\":\"uint256\",\"name\":\"_maxCost\",\"type\":\"uint256\"},{\"internalType\":\"address\",\"name\":\"_fees\",\"type\":\"address\"},{\"internalType\":\"bytes\",\"name\":\"_data\",\"type\":\"bytes\"}],\"name\":\"fillOrKillOrder\",\"outputs\":[],\"stateMutability\":\"view\",\"type\":\"function\"},{\"inputs\":[],\"name\":\"otherMethods\",\"outputs\":[],\"stateMutability\":\"nonpayable\",\"type\":\"function\"}]";//We'd need to know this ahead of time (or I hard code it with a default?)
	//Use 2 stage Recursion to be able to handle this Wrapping across this object and the Arg object
	TArray<TSharedPtr<FJsonValue>> Args;//We need to support nesting here!
	//This could be done like abi either enum or FString
	FString Func = "";
    
	FString GetJsonString() const;
};

UCLASS(BlueprintType)
class SEQUENCEPLUGIN_API UDelayedEncodingBP : public UObject
{
	GENERATED_BODY()
private:
	FDelayedEncoding DlyEncObject;
public:

	//create an enum'd version of this with a short list of common abi use cases (maybe)
	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void SetAbi(const FString& Abi);

	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void SetArgs(UDelayedEncodingArgsBP * ArgsIn);

	//create an enum'd version of this with a short list of common abi use cases
	UFUNCTION(BlueprintCallable, Category="Delayed Encoding")
	void SetFunc(const FString& Func);

	FString GetJsonString() const;

	FDelayedEncoding GetDelayedEncoding();
};