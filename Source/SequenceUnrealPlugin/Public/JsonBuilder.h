#pragma once

class FJsonArray;
class FJsonBuilder
{
	FString StringValue;
public:
	FString ToString() const;
	FJsonBuilder* AddField(FString Name, FString Value); // Adds the value raw
	FJsonBuilder* AddString(FString Name, FString Value);
	FJsonBuilder* AddInt(FString Name, int Value);
	FJsonBuilder* AddBool(FString Name, bool Value);
	FJsonArray AddArray(FString Name);

	// Conversion Functions
	static inline FString ConvertString(FString Value);
	static inline FString ConvertInt(int Value);
	static inline FString ConvertBool(bool Value);
};

class FJsonArray
{
	FString Name;
	FString StringValue;
	FJsonBuilder* Parent;
public:
	FJsonArray(FJsonBuilder* Parent, FString Name);
	FJsonArray* AddValue(FString Value);
	FJsonArray* AddString(FString Value);
	FJsonArray* AddBool(bool Value);
	FJsonArray* AddInt(int Value);
	FJsonBuilder* EndArray() const;
};