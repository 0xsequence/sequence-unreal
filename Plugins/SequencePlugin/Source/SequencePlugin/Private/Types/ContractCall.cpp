// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Types/ContractCall.h"
#include "Util/JsonBuilder.h"
#include "Util/SequenceSupport.h"

FString FContractCall::GetJson()
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable<FJsonObject>(new FJsonObject);
	if (!this->From.IsEmpty())
		JsonObject.Get()->SetStringField("from", this->From);

	if (!this->To.IsEmpty())
		JsonObject.Get()->SetStringField("to", this->To);

	if (this->Gas >= 0)
		JsonObject.Get()->SetNumberField("gas", this->Gas);

	if (this->GasPrice >= 0)
		JsonObject.Get()->SetNumberField("gasPrice", this->GasPrice);

	if (this->Value >= 0)
		JsonObject.Get()->SetNumberField("value", this->Value);
		
	if (!this->Data.IsEmpty())
		JsonObject.Get()->SetStringField("data", this->Data);
	
	return USequenceSupport::JsonToString(JsonObject);
}
