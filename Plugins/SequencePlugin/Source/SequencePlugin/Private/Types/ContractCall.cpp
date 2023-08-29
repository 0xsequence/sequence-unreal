#include "Types/ContractCall.h"

#include "Util/HexUtility.h"
#include "Util/JsonBuilder.h"

FString FContractCall::GetJson()
{
	auto JSON = FJsonBuilder();

	if(From.IsSet()) JSON.AddString("from", "0x" + From.GetValue().ToHex());
	JSON.AddString("to", "0x" + To.ToHex());
	if(Gas.IsSet()) JSON.AddString("gas", FString::FromInt(Gas.GetValue()));
	if(GasPrice.IsSet()) JSON.AddString("gasPrice", FString::FromInt(GasPrice.GetValue()));
	if(Value.IsSet()) JSON.AddString("value", IntToHexString(Value.GetValue()));
	if(Data.IsSet()) JSON.AddString("data", "0x" + Data.GetValue());

	return JSON.ToString();
}
