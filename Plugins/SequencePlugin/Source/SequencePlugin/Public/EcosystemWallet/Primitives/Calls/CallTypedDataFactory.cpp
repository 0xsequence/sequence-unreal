#include "CallTypedDataFactory.h"
#include "JsonObjectConverter.h"

FString FCallTypedDataFactory::FromCalls(const FString& Wallet, const FBigInt& ChainId, const TSharedPtr<FCalls>& Calls)
{
	FCallTypedDataParent Data;

    // Domain
	
    Data.Domain.Name = TEXT("Sequence Wallet");
    Data.Domain.Version = TEXT("3");
    Data.Domain.ChainId = FCString::Atoi64(*ChainId.Value);
    Data.Domain.VerifyingContract = Wallet;

	FEIP712TypeEntry CallsEntry1;
	CallsEntry1.Name = TEXT("calls");
	CallsEntry1.Type = TEXT("Call[]");

	FEIP712TypeEntry CallsEntry2;
	CallsEntry2.Name = TEXT("space");
	CallsEntry2.Type = TEXT("uint256");

	FEIP712TypeEntry CallsEntry3;
	CallsEntry3.Name = TEXT("nonce");
	CallsEntry3.Type = TEXT("uint256");

	FEIP712TypeEntry CallsEntry4;
	CallsEntry4.Name = TEXT("wallets");
	CallsEntry4.Type = TEXT("address[]");

	Data.Types.Calls = { CallsEntry1, CallsEntry2, CallsEntry3, CallsEntry4 };

	// Call type
	
	FEIP712TypeEntry CallEntry1;
	CallEntry1.Name = TEXT("to");
	CallEntry1.Type = TEXT("address");

	FEIP712TypeEntry CallEntry2;
	CallEntry2.Name = TEXT("value");
	CallEntry2.Type = TEXT("uint256");

	FEIP712TypeEntry CallEntry3;
	CallEntry3.Name = TEXT("data");
	CallEntry3.Type = TEXT("bytes");

	FEIP712TypeEntry CallEntry4;
	CallEntry4.Name = TEXT("gasLimit");
	CallEntry4.Type = TEXT("uint256");

	FEIP712TypeEntry CallEntry5;
	CallEntry5.Name = TEXT("delegateCall");
	CallEntry5.Type = TEXT("bool");

	FEIP712TypeEntry CallEntry6;
	CallEntry6.Name = TEXT("onlyFallback");
	CallEntry6.Type = TEXT("bool");

	FEIP712TypeEntry CallEntry7;
	CallEntry7.Name = TEXT("behaviorOnError");
	CallEntry7.Type = TEXT("uint256");

	Data.Types.Call = { CallEntry1, CallEntry2, CallEntry3, CallEntry4, CallEntry5, CallEntry6, CallEntry7 };

    Data.PrimaryType = TEXT("Calls");

	// Message

	const FCalls* Calls2 = Calls.Get();
    Data.Message.Space = Calls2->Space.Value;
    Data.Message.Nonce = Calls2->Nonce.Value;
    Data.Message.Wallets = {};

	for (const FCall& OriginalCall : Calls2->Calls)
	{
		FTypedCall Call;
		Call.To = OriginalCall.To;
		Call.Value = FCString::Atoi64(*OriginalCall.Value.Value);
		Call.Data = OriginalCall.Data;
		Call.GasLimit = FCString::Atoi64(*OriginalCall.GasLimit.Value);
		Call.DelegateCall = OriginalCall.DelegateCall;
		Call.OnlyFallback = OriginalCall.OnlyFallback;
		Call.BehaviorOnError = FCString::Atoi64(*OriginalCall.BehaviorOnError);

		Data.Message.Calls.Add(Call);
	}

	TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();

	// Add "domain"
	Root->SetObjectField("domain", FJsonObjectConverter::UStructToJsonObject(Data.Domain));

	// Add "types"
	TSharedPtr<FJsonObject> TypesObj = MakeShared<FJsonObject>();
	TypesObj->SetArrayField("Calls", StructArrayToJsonArray(Data.Types.Calls));
	TypesObj->SetArrayField("Call", StructArrayToJsonArray(Data.Types.Call));
	Root->SetObjectField("types", TypesObj);

	// Add "primaryType"
	Root->SetStringField("primaryType", Data.PrimaryType);

	// Add "message"
	Root->SetObjectField("message", FJsonObjectConverter::UStructToJsonObject(Data.Message));

	FString JsonOut;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonOut);
	FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);

    return JsonOut;
}
