// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Containers/UnrealString.h"

// Conversion Functions
FString ConvertString(FString Value);
FString ConvertInt(int Value);
FString ConvertBool(bool Value);

class FJsonArray;
class FJsonBuilder
{
public:
	FString StringValue;
	FString ToString() const;
	FJsonBuilder* ToPtr();
	FJsonBuilder* AddField(FString Name, FString Value); // Adds the value raw
	FJsonBuilder* AddString(FString Name, FString Value);
	FJsonBuilder* AddInt(FString Name, int Value);
	FJsonBuilder* AddBool(FString Name, bool Value);
	FJsonArray AddArray(FString Name);
};

class FJsonArray
{
	FString Name;
	FString StringValue;
	FJsonBuilder* Parent;
public:
	FJsonArray();
	FJsonArray(FJsonBuilder* Parent, FString Name);
	FJsonArray* ToPtr();
	FJsonArray* AddValue(FString Value);
	FJsonArray* AddString(FString Value);
	FJsonArray* AddBool(bool Value);
	FJsonArray* AddInt(int Value);
	FJsonBuilder* EndArray() const;
	FString ToString() const;
};