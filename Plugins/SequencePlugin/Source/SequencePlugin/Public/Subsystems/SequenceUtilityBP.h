#pragma once

#include "EcosystemWallet/Transactions/Transaction.h"
#include "Sequence/Transactions.h"
#include "Util/SeqLogVerbosity.h"
#include "SequenceUtilityBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceUtilityBP : public UObject
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDecodeResponse, const FString&, Json);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	
public:
	USequenceUtilityBP();
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static void OpenExternalBrowser(const FString& Url);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static UTransactions* ConstructSingleERC20Transaction(const FString& ContractAddress, const FString& RecipientAddress, const FString& Value);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static UTransactions* ConstructSingleERC721Transaction(const FString& ContractAddress, const FString& RecipientAddress, const FString& TokenId);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static UTransactions* ConstructSingleERC1155Transaction(const FString& ContractAddress, const FString& RecipientAddress, const FString& TokenId, const FString& Amount);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static TScriptInterface<ISeqTransactionBase> ConvertToEcosystemWalletTransaction(const FRawTransaction& RawTransaction);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static TScriptInterface<ISeqTransactionBase> ConvertToEcosystemWalletTransactions(const TArray<FRawTransaction>& RawTransactions);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static void ClipboardCopy(const FString& Text);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static float GetUserReadableAmountIntDecimals(int64 Amount, int64 Decimals);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static int64 GetTransactionReadableAmountIntDecimals(float Amount, int64 Decimals);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static int64 GetNetworkIdFromName(const FString& NetworkNameIn);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Utils")
	static int64 GetNetworkIdFromNetworkEnum(const ENetwork& NetworkEnumIn);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static FString GetNetworkNameFromId(const int64 NetworkIdIn);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Utils")
	static FString GetNetworkNameFromEnum(const ENetwork NetworkIdIn);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static TArray<FIdNamePair> GetAllNetworks();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static TArray<FString> GetAllNetworkNames();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static TArray<int64> GetAllNetworkIds();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils", meta = (ToolTip = "Encode function parameters based on the function signature. Such as 'balanceOf(address,uint256)' with '[\"0x6615e4e985bf0d137196897dfa182dbd7127f54f\", 2]'"))
	static FString EncodeFunctionData(const FString& FunctionSignature, const FString& Values);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils", meta = (ToolTip = "Decode any encoded data you receive from a Call() function. ABI Example: '{\"type\": \"function\", ...}'"))
	static FString DecodeFunctionResult(const FString& Abi, const FString& EncodedData);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static void MakeSequenceLog(const ESeqLogVerbosity Verbosity, const FString& Message);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Utils")
	static void MakeSequenceEditorLog(const ESeqLogVerbosity Verbosity, const FString& Message);
};
