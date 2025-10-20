#include "TransactionService.h"
#include "SignatureService.h"
#include "SignerService.h"
#include "EcosystemWallet/Primitives/Envelope/Envelope.h"
#include "EcosystemWallet/Signers/UsageLimit.h"
#include "Extensions/ExtensionsFactory.h"
#include "Util/SequenceSupport.h"

void FTransactionService::SignAndBuild(FBigInt ChainId, const TArray<FCall>& Calls, const bool CheckDeployed, TFunction<void(TTuple<FString, FString>)> OnSuccess, TFunction<void(FString)> OnFailure)
{
	const TSharedPtr<FTransactionService> ServicePtr = MakeShared<FTransactionService>(*this);
	
	PrepareIncrement(ServicePtr, ChainId, Calls, [ChainId, Calls, CheckDeployed, OnSuccess, OnFailure](const TSharedPtr<FTransactionService>& Service, const TSharedPtr<FCall>& Increment)
	{
		TArray<FCall> FinalCalls = Calls;
		if (Increment != nullptr && Increment.IsValid())
		{
			FinalCalls.Insert(*Increment.Get(), 0);
		}

		if (Service == nullptr || !Service.IsValid())
		{
			OnFailure(TEXT("Wallet state is invalid"));
			return;
		}

		FWalletState* CurrentWalletState = Service->WalletState.Get();
		TSharedPtr<FCalls> Payload = MakeShared<FCalls>(FinalCalls, FBigInt("0"), FBigInt(CurrentWalletState->Nonce));
		TSharedPtr<FEnvelope> Envelope = MakeShared<FEnvelope>(ChainId, CurrentWalletState->Address, CurrentWalletState->Config, Payload);
		
		TSharedPtr<FSignatureService> SignatureService = MakeShared<FSignatureService>(FSignatureService(ChainId, CurrentWalletState->SessionsImageHash, Envelope,
		CurrentWalletState->ConfigUpdates, Service->Signers, CurrentWalletState->SessionsTopology));
	
		SignatureService->SignCalls([Payload, CurrentWalletState, CheckDeployed, OnSuccess, OnFailure](const TSharedPtr<FRawSignature>& Signature)
		{
			if (!Payload.IsValid() || !Signature.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("Payload or Signature is a null ptr"));
				OnFailure(TEXT("Payload or Signature is a null ptr"));
				return;
			}

			if (CurrentWalletState == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("wallet state is a null ptr"));
				OnFailure(TEXT("wallet state is a null ptr"));
				return;
			}
			
			const FString EncodedPayload = FByteArrayUtils::BytesToHexString(Payload->Encode());
			const FString EncodedSignature = FByteArrayUtils::BytesToHexString(Signature->Encode());
			
			const FString ExecuteValues = FString::Printf(TEXT("[\"%s\",\"%s\"]"), *EncodedPayload, *EncodedSignature);
			const FString EncodedExecute = FEthAbiBridge::EncodeFunctionCall("execute(bytes,bytes)", ExecuteValues);
			
			if (CurrentWalletState->IsDeployed || !CheckDeployed)
			{
				OnSuccess(TTuple<FString, FString>(CurrentWalletState->Address, EncodedExecute));
				return;
			}

			const TArray<uint8> DeployData = FErc6492::CreateDeployData(CurrentWalletState->DeployContext.MainModule, CurrentWalletState->DeployHash);
			const FString DeployDataHex = FByteArrayUtils::BytesToHexString(DeployData);

			const TArray<FCall> DeployCalls {
				FCall {CurrentWalletState->DeployContext.Factory, DeployDataHex, FBigInt("0"), FBigInt("0"), false, false, "1"},
				FCall {CurrentWalletState->Address, EncodedExecute, FBigInt("0"), FBigInt("0"), false, false, "1"}
			};

			FCalls DeployPayload = FCalls(DeployCalls, FBigInt("0"), FBigInt("0"));
			const FString EncodedDeployPayload = FByteArrayUtils::BytesToHexString(DeployPayload.Encode());

			OnSuccess(TTuple<FString, FString>(CurrentWalletState->DeployContext.GuestModule, EncodedDeployPayload));
		}, OnFailure);	
	});
}

void FTransactionService::PrepareIncrement(const TSharedPtr<FTransactionService>& Service, const FBigInt& ChainId, const TArray<FCall>& Calls, const TFunction<void(TSharedPtr<FTransactionService>, TSharedPtr<FCall>)>& OnCompleted)
{
    TSharedPtr<FSignerService> SignerService = MakeShared<FSignerService>(ChainId, Calls, Signers, WalletState->SessionsTopology);
    SignerService->FindSignersForCalls([Service, ChainId, Calls, OnCompleted](const TArray<FSessionSigner>& SupportedSigners)
    {
    	// Bind calls to each signer
    	TMap<TSharedPtr<FSessionSigner>, TArray<FCall>> SignersToCallsMap;
		for (int32 i = 0; i < SupportedSigners.Num(); i++)
		{
			const TSharedPtr<FSessionSigner> Signer = MakeShared<FSessionSigner>(SupportedSigners[i]);
			if (SignersToCallsMap.Contains(Signer))
			{
				SignersToCallsMap[Signer].Add(Calls[i]);
			}
			else
			{
				SignersToCallsMap.Add(Signer, { Calls[i] });
			}
		}
    	
    	// Let signers create the increment for their calls
    	WaitForAllIncrements(ChainId, Service->WalletState->SessionsTopology, SignersToCallsMap, [Service, OnCompleted](const TArray<TSharedPtr<FUsageLimit>>& Increments)
    	{
    		TArray<TSharedPtr<FUsageLimit>> ValidIncrements;
    		for (int32 i = 0; i < Increments.Num(); i++)
    		{
    			TSharedPtr<FUsageLimit> Increment = Increments[i];
    			if (Increment != nullptr && Increment.IsValid())
    			{
    				ValidIncrements.Add(Increment);
    			}
    		}
    		
			if (ValidIncrements.Num() == 0)
			{
				OnCompleted(Service, nullptr);
				return;
			}
    		
			const FString UsageHash = ValidIncrements[0]->UsageHash;
			const FString UsageAmount = ValidIncrements[0]->UsageAmount.Value;
			
			const FString FunctionAbi = "incrementUsageLimit((bytes32,uint256)[])";
			const FString Values = FString::Printf(TEXT("[[[\"%s\",%s]]]"), *UsageHash, *UsageAmount);
    		
			const FString Result = USequenceSupport::EncodeFunctionCall(FunctionAbi, Values);
			const TArray<uint8> CallData = FByteArrayUtils::HexStringToBytes(Result);

			FCall Call {
				FExtensionsFactory::GetCurrent().Sessions,
				FByteArrayUtils::BytesToHexString(CallData),
				FBigInt("0"),
				FBigInt("0"),
				false, false, "1",
			};

			UE_LOG(LogTemp, Display, TEXT("Increment call data %s"), *FByteArrayUtils::BytesToHexString(CallData));
    		
			OnCompleted(Service, MakeShared<FCall>(Call));
    	});
    }, [OnCompleted](const FString& Error)
    {
    	UE_LOG(LogTemp, Error, TEXT("Transaction service failed to prepare increment %s"), *Error);
	    OnCompleted(nullptr, nullptr);
    });
}

void FTransactionService::WaitForAllIncrements(
	const FBigInt& ChainId,
	const TSharedPtr<FSessionsTopology>& SessionsTopology,
	const TMap<TSharedPtr<FSessionSigner>, TArray<FCall>>& SignersToCallsMap,
	TFunction<void(const TArray<TSharedPtr<FUsageLimit>>&)> OnAllComplete)
{
	if (SignersToCallsMap.Num() == 0)
	{
		OnAllComplete({});
		return;
	}

	TSharedRef<int32> Remaining = MakeShared<int32>(SignersToCallsMap.Num());
	TSharedRef<TArray<TSharedPtr<FUsageLimit>>> Results = MakeShared<TArray<TSharedPtr<FUsageLimit>>>();
	Results->SetNum(SignersToCallsMap.Num());

	auto SignersToCallsArray = SignersToCallsMap.Array();
	for (int32 i = 0; i < SignersToCallsArray.Num(); i++)
	{
		SignersToCallsArray[i].Key->PrepareIncrements(ChainId, SignersToCallsArray[i].Value, SessionsTopology, [Remaining, Results, i, OnAllComplete](const TSharedPtr<FUsageLimit>& Increment)
		{
			(*Results)[i] = Increment;
			(*Remaining)--;
			
			if (*Remaining == 0)
			{
				OnAllComplete(*Results);
			}
		});
	}
}
