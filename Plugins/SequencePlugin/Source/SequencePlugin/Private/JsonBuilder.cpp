#include "JsonBuilder.h"

FString ConvertString(const FString Value)
{
	return "\"" + Value + "\"";
}

FString ConvertInt(const int Value)
{
	return FString::FromInt(Value);
}

FString ConvertBool(const bool Value)
{
	return Value ? "true" : "false";
}

FJsonBuilder* FJsonBuilder::AddField(const FString Name, const FString Value)
{
	if(!StringValue.Equals(""))
	{
		StringValue = StringValue + ", ";
	}

	StringValue = StringValue + ConvertString(Name) + ":" + Value;
	return this;
}

FJsonBuilder* FJsonBuilder::AddString(const FString Name, const FString Value)
{
	AddField(Name, ConvertString(Value));
	return this;
}

FJsonBuilder* FJsonBuilder::AddInt(const FString Name, const int Value)
{
	AddField(Name, ConvertInt(Value));
	return this;
}

FJsonBuilder* FJsonBuilder::AddBool(const FString Name, const bool Value)
{
	AddField(Name, ConvertBool(Value));
	return this;
}

FJsonArray FJsonBuilder::AddArray(FString Name)
{
	return FJsonArray(this, Name);
}

FString FJsonBuilder::ToString() const
{
	auto x = 3;
	return FString("{") + StringValue + FString("}");
}

FJsonBuilder* FJsonBuilder::ToPtr()
{
	return this;
}



FJsonArray::FJsonArray(FJsonBuilder* Parent, const FString Name) : Name(Name), Parent(Parent)
{
	StringValue = "";
}

FJsonArray* FJsonArray::ToPtr()
{
	return this;
}

FJsonArray* FJsonArray::AddValue(FString Value)
{
	if(!StringValue.Equals(""))
	{
		StringValue = StringValue + ", ";
	}
	StringValue = StringValue + Value;
	return this;
}

FJsonArray* FJsonArray::AddString(FString Value)
{
	return AddValue(ConvertString(Value));
}

FJsonArray* FJsonArray::AddBool(bool Value)
{
	return  AddValue(ConvertBool(Value));
}

FJsonArray* FJsonArray::AddInt(int Value)
{
	return AddValue(ConvertInt(Value));
}

FJsonBuilder* FJsonArray::EndArray() const
{
	Parent->AddField(ConvertString(Name), "[" + StringValue + "]");
	return Parent;
}


