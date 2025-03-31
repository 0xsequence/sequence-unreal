// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Sequence/FeeOption.h"
#include "Sequence/SequenceFederationSupport.h"
#include "SequenceResponseIntent.generated.h"

//Error Response//
//{"error":"EmailAlreadyInUse","code":7000,"msg":"Could not create account as the email is already in use","cause":"OIDC|testbed995@gmail.com|https://accounts.google.com","status":409}

USTRUCT()
struct SEQUENCEPLUGIN_API FErrorResponse
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString Error = "";
	UPROPERTY()
	int32 Code = -1;
	UPROPERTY()
	FString Msg = "";
	UPROPERTY()
	FString Cause = "";
	UPROPERTY()
	int32 Status = -1;
	
	bool IsEmailInUseError() const
	{
		return Error.Equals(TEXT("EmailAlreadyInUse"),ESearchCase::IgnoreCase);
	}
	
	FString ParseCauseForRequiredEmail() const
	{
		FString Email = "";

		TArray<FString> PreParsed;
		Cause.ParseIntoArray(PreParsed,TEXT(","));

		for (FString Entry : PreParsed)
		{
			TArray<FString> PostParsed;
			Entry.ParseIntoArray(PostParsed,TEXT("|"));

			if (PostParsed.Num() >= 2)
			{
				Email = PostParsed[1];
				break;
			}
		}
		
		return Email;
	}
	
	TArray<TEnumAsByte<ESequenceLoginType>> ParseCauseForAccountUsage() const
	{
		TArray<TEnumAsByte<ESequenceLoginType>> AccountTypes;

		TArray<FString> PreParsed;
		Cause.ParseIntoArray(PreParsed,TEXT(","));

		for (FString Entry : PreParsed)
		{
			TArray<FString> PostParsed;
			Entry.ParseIntoArray(PostParsed,TEXT("|"));
			if (PostParsed.Num() >= 2)
			{
				if (PostParsed[0].Equals(TEXT("Email"),ESearchCase::IgnoreCase))
				{
					AccountTypes.Add(ESequenceLoginType::Email);
				}
				else if (PostParsed[0].Equals(TEXT("PlayFab"), ESearchCase::IgnoreCase))
				{
					AccountTypes.Add(ESequenceLoginType::PlayFab);
				}
				else if (PostParsed[0].Equals(TEXT("OIDC"),ESearchCase::IgnoreCase) && PostParsed.Num() >= 3)
				{
					//Now we check which type of OIDC
					if (PostParsed[2].Contains(TEXT("apple"),ESearchCase::IgnoreCase))
					{
						AccountTypes.Add(ESequenceLoginType::OIDC_Apple);
					}
					else if (PostParsed[2].Contains(TEXT("google"),ESearchCase::IgnoreCase))
					{
						AccountTypes.Add(ESequenceLoginType::OIDC_Google);
					}
				}
			}
		}
		
		return AccountTypes;
	}
};

//Error Response//

USTRUCT()
struct SEQUENCEPLUGIN_API FFederateAccountResponse_Account
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString Id = "";
	UPROPERTY()
	FString Type = "";
	UPROPERTY()
	FString Issuer = "";
	UPROPERTY()
	FString Email = "";
};

USTRUCT()
struct SEQUENCEPLUGIN_API FFederateAccountResponse_Data
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FFederateAccountResponse_Account Account;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FFederateAccountResponse_Response
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString Code = "";
	UPROPERTY()
	FFederateAccountResponse_Data Data;

	bool IsValid() const
	{
		return Code.Equals(TEXT("accountFederated"),ESearchCase::IgnoreCase);
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FFederateAccountResponse
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	FFederateAccountResponse_Response Response;

	bool IsValid() const
	{
		return Response.IsValid();
	}
};

//FederateAccount//

//GetFeeOptions//

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqGetFeeOptionsResponse_Data
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FFeeOption> FeeOptions;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqGetFeeOptionsResponse_Response
{
	GENERATED_BODY()
	UPROPERTY()
	FString Code = "";
	UPROPERTY()
	FSeqGetFeeOptionsResponse_Data Data;
	UPROPERTY()
	FString FeeQuote = "";

	bool IsValid() const
	{
		return Code.Equals(TEXT("feeOptions"),ESearchCase::IgnoreCase);
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqGetFeeOptionsResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FSeqGetFeeOptionsResponse_Response Response;

	bool IsValid() const
	{
		return Response.IsValid();
	}
};

//GetFeeOptions//

//CloseSession//

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqCloseSessionResponse_Response
{
	GENERATED_BODY()

	UPROPERTY()
	FString Code = "";

	bool IsValid() const
	{
		return Code.Equals(TEXT("sessionClosed"),ESearchCase::IgnoreCase);
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqCloseSessionResponse
{
	GENERATED_BODY()
	UPROPERTY()
	FSeqCloseSessionResponse_Response Response;

	bool IsValid() const
	{
		return Response.IsValid();
	}
};

//CloseSession//

//SendTransaction//

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTxnLog
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Address = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString BlockHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString BlockNumber = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Data = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString LogIndex = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	bool Removed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	TArray<FString> Topics;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString TransactionHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString TransactionIndex = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_NativeReceipt
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString BlockHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString BlockNumber = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString ContractAddress = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString CumulativeGasUsed = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString GasUsed = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	TArray<FSeqTxnLog> Logs;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString LogsBloom = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Root = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Status = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString TransactionHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString TransactionIndex = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTxnSimulation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	bool Executed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	int64 GasLimit = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	int64 GasUsed = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Reason = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Result = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	bool Succeeded = false;
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTxnSignature
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString SessionId = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Signature = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_Transaction
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString To = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString TokenAddress = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Type = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Value = "";	
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTxnData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Identifier = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Network = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	TArray<FSeqTransactionResponse_Transaction> Transactions;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString TransactionFeeQuote = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Wallet = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_Request
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FSeqTxnData Data;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	int64 ExpiresAt = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	int64 IssuedAt = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Name = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	TArray<FSeqTxnSignature> Signatures;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Version = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_Receipt
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Id = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	int64 Index = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	TArray<FSeqTxnLog> Logs;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString RevertReason = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Status = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString TxnReceipt = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_Data
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString IdentifyingCode = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString TxHash = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString MetaTxHash = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FSeqTransactionResponse_Request Request;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FSeqTransactionResponse_NativeReceipt NativeReceipt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FSeqTransactionResponse_Receipt Receipt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	TArray<FSeqTxnSimulation> Simulations;

	//Raw Json Response for ease of use with older code
	TSharedPtr<FJsonObject> Json;

	void Setup(const TSharedPtr<FJsonObject>& JsonIn)
	{
		Json = JsonIn;
	}
};

//transactionReceipt

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_Response
{
	GENERATED_BODY()
	UPROPERTY()
	FString Code = "";
	UPROPERTY()
	FSeqTransactionResponse_Data Data;

	bool IsValid() const
	{
		return Code.Equals(TEXT("transactionReceipt"),ESearchCase::IgnoreCase);
	}
	
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqTransactionResponse
{
	GENERATED_BODY()
	
	UPROPERTY()
	FSeqTransactionResponse_Response Response;

	bool IsValid() const
	{
		return Response.IsValid();
	}
};

//SendTransaction//

//SignMessage//

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqSignMessageResponse_Data
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Message = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Signature = "";
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqSignMessageResponse_Response
{
	GENERATED_BODY()
	UPROPERTY()
	FString Code = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FSeqSignMessageResponse_Data Data;

	bool IsValid() const
	{
		return Code.Equals(TEXT("signedMessage"),ESearchCase::IgnoreCase);
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqSignMessageResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FSeqSignMessageResponse_Response Response;

	bool IsValid() const
	{
		return Response.IsValid();
	}
};

//SignMessage//


//GetIdToken//

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqIdTokenResponse_Data
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString IdToken;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	int32 ExpiresIn;
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqIdTokenResponse_Response
{
	GENERATED_BODY()

	UPROPERTY()
	FString Code = "";

	UPROPERTY()
	FSeqIdTokenResponse_Data Data;


	bool IsValid() const
	{
		return Code.Equals(TEXT("idToken"), ESearchCase::IgnoreCase);

	}
};


USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqIdTokenResponse
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FSeqIdTokenResponse_Response Response;

	bool IsValid() const
	{
		return Response.IsValid();
	}
};

// Wallet Linking

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqLinkedWalletRequest
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, meta = (JsonFieldName = "signatureChainId"), Category = "0xSequence")
	FString SignatureChainId = "";
	
	UPROPERTY(BlueprintReadWrite, meta = (JsonFieldName = "parentWalletAddress"), Category = "0xSequence")
	FString ParentWalletAddress = "";
	
	UPROPERTY(BlueprintReadWrite, meta = (JsonFieldName = "parentWalletMessage"), Category = "0xSequence")
	FString ParentWalletMessage = "";

	UPROPERTY(BlueprintReadWrite, meta = (JsonFieldName = "parentWalletSignature"), Category = "0xSequence")
	FString ParentWalletSignature = "";

	UPROPERTY(BlueprintReadWrite, meta = (JsonFieldName = "linkedWalletAddress"), Category = "0xSequence")
	FString LinkedWalletAddress = "";
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqLinkedWallet
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, meta = (JsonFieldName = "id"), Category = "0xSequence")
	int64 Id = 0;
	
	UPROPERTY(BlueprintReadWrite, meta = (JsonFieldName = "walletType"), Category = "0xSequence")
	FString WalletType = "";
	
	UPROPERTY(BlueprintReadWrite, meta = (JsonFieldName = "walletAddress"), Category = "0xSequence")
	FString WalletAddress = "";

	UPROPERTY(BlueprintReadWrite, meta = (JsonFieldName = "linkedWalletAddress"), Category = "0xSequence")
	FString LinkedWalletAddress = "";

	UPROPERTY(BlueprintReadWrite, meta = (JsonFieldName = "createdAt"), Category = "0xSequence")
	FString CreatedAt = "";
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqLinkedWalletsResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, meta = (JsonFieldName = "linkedWallets"), Category = "0xSequence")
	TArray<FSeqLinkedWallet> LinkedWallets;
};


//Validate Signature//

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqValidateMessageSignatureResponse_Data
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	bool isValid;

	bool IsValid() const
	{
		return isValid;
	}
};



USTRUCT()
struct SEQUENCEPLUGIN_API FSeqValidateMessageSignatureResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FSeqValidateMessageSignatureResponse_Data Data;

	bool IsValid() const
	{
		return Data.IsValid();
	}
};
 
// 
//ListSessions//

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqListSessions_Identity
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Type = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Iss = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString Sub = "";
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqListSessions_Session
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Id = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	int64 ProjectId = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString UserId = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FSeqListSessions_Identity Identity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString FriendlyName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString CreatedAt = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString RefreshedAt = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString ExpiresAt = "";
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqListSessionResponse_Response
{
	GENERATED_BODY()
	UPROPERTY()
	FString Code = "";
	UPROPERTY()
	TArray<FSeqListSessions_Session> Data;

	bool IsValid() const
	{
		return Code.Equals(TEXT("sessionList"),ESearchCase::IgnoreCase);
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqListSessionsResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FSeqListSessionResponse_Response Response;

	bool IsValid() const
	{
		return Response.IsValid();
	}
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqAccount
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Id = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Type = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Issuer = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Email = "";


};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqListAccountsResponse_Data
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	TArray<FSeqAccount> Accounts;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
	FString CurrentAccountId = "";
};



USTRUCT()
struct SEQUENCEPLUGIN_API FSeqListAccountsResponse_Response
{
	GENERATED_BODY()
	UPROPERTY()
	FString Code = "";
	UPROPERTY()
	FSeqListAccountsResponse_Data Data;

	bool IsValid() const
	{
		return Code.Equals(TEXT("accountList"), ESearchCase::IgnoreCase);
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqListAccountsResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FSeqListAccountsResponse_Response Response;

	bool IsValid() const
	{
		return Response.IsValid();
	}
};


//ListSessions//


USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqGetSessionAuthProof_Data
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString SessionId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Network;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Wallet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Message; 	//The message contents : �SessionAuthProof <sessionId> <wallet> <nonce ? >� hex encoded
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Signature;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqGetSessionAuthProofResponse_Response
{
	GENERATED_BODY()
	UPROPERTY()
	FString Code = "";
	UPROPERTY()
	FSeqGetSessionAuthProof_Data Data;

	bool IsValid() const
	{
		return Code.Equals(TEXT("sessionAuthProof"), ESearchCase::IgnoreCase);
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqGetSessionAuthProofResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FSeqGetSessionAuthProofResponse_Response Response;

	bool IsValid() const
	{
		return Response.IsValid();
	}
};