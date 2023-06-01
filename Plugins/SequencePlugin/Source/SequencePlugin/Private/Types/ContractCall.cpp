#include "Types/ContractCall.h"

#include "JsonBuilder.h"

FString ContractCall::GetJson()
{
	auto json = FJsonBuilder();

	if(From.IsSet()) json.AddString("from", "0x" + From.GetValue().ToHex());
	json.AddString("to", "0x" + To.ToHex());
	if(Gas.IsSet()) json.AddString("gas", FString::FromInt(Gas.GetValue()));
	if(GasPrice.IsSet()) json.AddString("gasPrice", FString::FromInt(GasPrice.GetValue()));
	if(Value.IsSet()) json.AddString("value", FString::FromInt(Value.GetValue()));
	if(Data.IsSet()) json.AddString("data", "0x" + Data.GetValue().ToHex());

	return json.ToString();
}
