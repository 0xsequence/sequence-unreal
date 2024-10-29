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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Address = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString BlockHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString BlockNumber = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Data = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString LogIndex = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool Removed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FString> Topics;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TransactionHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TransactionIndex = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_NativeReceipt
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString BlockHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString BlockNumber = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString ContractAddress = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString CumulativeGasUsed = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString GasUsed = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FSeqTxnLog> Logs;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString LogsBloom = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Root = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Status = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TransactionHash = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TransactionIndex = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTxnSimulation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool Executed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int64 GasLimit = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int64 GasUsed = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Reason = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Result = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool Succeeded = false;
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTxnSignature
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString SessionId = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Signature = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_Transaction
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString To = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TokenAddress = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Type = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Value = "";	
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTxnData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Identifier = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Network = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FSeqTransactionResponse_Transaction> Transactions;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TransactionFeeQuote = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Wallet = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_Request
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FSeqTxnData Data;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int64 ExpiresAt = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int64 IssuedAt = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Name = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FSeqTxnSignature> Signatures;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Version = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_Receipt
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Id = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int64 Index = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FSeqTxnLog> Logs;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString RevertReason = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Status = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString TxnReceipt = "";
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionResponse_Data
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString IdentifyingCode = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString TxHash = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString MetaTxHash = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FSeqTransactionResponse_Request Request;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FSeqTransactionResponse_NativeReceipt NativeReceipt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FSeqTransactionResponse_Receipt Receipt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Message = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Signature = "";
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqSignMessageResponse_Response
{
	GENERATED_BODY()
	UPROPERTY()
	FString Code = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString IdToken;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
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


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FSeqIdTokenResponse_Response Response;

	bool IsValid() const
	{
		return Response.IsValid();
	}
};


//Validate Signature//

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqValidateMessageSignatureResponse_Data
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Type = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Iss = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString Sub = "";
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqListSessions_Session
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString Id = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	int64 ProjectId = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString UserId = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FSeqListSessions_Identity Identity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString FriendlyName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString CreatedAt = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString RefreshedAt = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
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
		return Code.Equals(TEXT("session"),ESearchCase::IgnoreCase);
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString Id = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString Type = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString Issuer = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString Email = "";


};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSeqListAccountsResponse_Data
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FSeqAccount> Accounts;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString SessionId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString Network;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString Wallet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString Message; 	//The message contents : �SessionAuthProof <sessionId> <wallet> <nonce ? >� hex encoded
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
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