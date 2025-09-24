#include "EcosystemWallet/Transactions/ContractTransaction.h"
#include "EcosystemWallet/Primitives/Calls//Call.h"
#include "EcosystemWallet/Primitives/Calls/Calls.h"
#include "Util/SequenceSupport.h"

UContractTransaction* UContractTransaction::CreateContractTransaction(const FString& To, const int64 Value, const FString& FunctionSignature, const TArray<FString> Values)
{
	UContractTransaction* Permissions = NewObject<UContractTransaction>();
	Permissions->To = To;
	Permissions->Value = Value;
	Permissions->FunctionSignature = FunctionSignature;
	Permissions->Values = Values;

	return Permissions;
}

FCalls UContractTransaction::GetCalls()
{
	TArray<TSharedPtr<FJsonValue>> Values;
	for (const auto& Value : this->Values)
	{
		// Convert each string value to its correct type
		Values.Add(MakeShareable(new FJsonValueString(Value)));
	}

	const FString JsonValues = USequenceSupport::JsonValueListToParsableString(Values);
	FString Data = USequenceSupport::EncodeFunctionCall(this->FunctionSignature, JsonValues);
	
	FCall Call;
	Call.To = this->To;
	Call.Data = Data;
	Call.Value = this->Value;
	Call.GasLimit = 0;
	Call.DelegateCall = false;
	Call.OnlyFallback = false;

	FCalls Calls;
	Calls.Calls.Add(Call);

	return Calls;
}
