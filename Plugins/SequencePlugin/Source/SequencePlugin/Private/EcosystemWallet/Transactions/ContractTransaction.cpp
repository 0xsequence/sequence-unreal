#include "EcosystemWallet/Transactions/ContractTransaction.h"
#include "EcosystemWallet/Primitives/Calls//Call.h"
#include "EcosystemWallet/Primitives/Calls/Calls.h"
#include "Util/SequenceSupport.h"

UContractTransaction* UContractTransaction::CreateContractTransaction(const FString& To, const FString& Value, const FString& FunctionSignature, const TArray<FString> Values)
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
	
	FCall Call {
		this->To,
		Data,
		FBigInt(this->Value),
		FBigInt("0"),
		false,
		false,
		"1"
	};
	
	FCalls Calls;
	Calls.Calls.Add(Call);

	return Calls;
}
