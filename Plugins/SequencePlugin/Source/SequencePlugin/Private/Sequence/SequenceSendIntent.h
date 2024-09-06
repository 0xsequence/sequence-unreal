// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "ConfigFetcher.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Containers/Union.h"
#include "Util/SequenceSupport.h"
#include "SequenceSendIntent.generated.h"

//Operations Constants//

static const FString OpenSessionOP = "openSession";
static const FString InitiateAuthOP = "initiateAuth";
static const FString CloseSessionOP = "closeSession";
static const FString SendTransactionOP = "sendTransaction";
static const FString FeeOptionsOP = "feeOptions";
static const FString ListSessionsOP = "listSessions";
static const FString SignMessageOP = "signMessage";
static const FString FederateSessionOP = "federateAccount";

static const FString EmailType = "Email";
static const FString GuestType = "Guest";
static const FString OIDCType = "OIDC";
static const FString PlayFabType = "PlayFab";

//Operations Constants//

USTRUCT()
struct SEQUENCEPLUGIN_API FGenericData
{
 FString Operation = "";
 bool UseCustomParser = false;
 GENERATED_USTRUCT_BODY()
 FGenericData(){}
 virtual FString GetJson() const
 {
  return "";
 }
 virtual ~FGenericData(){}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FOpenSessionData : public FGenericData
{
 GENERATED_USTRUCT_BODY()
 
 UPROPERTY()
 FString answer = "";
 UPROPERTY()
 bool forceCreateAccount = false;
 UPROPERTY()
 FString identityType = "";
 UPROPERTY()
 FString sessionId = "";
 UPROPERTY()
 FString verifier = "";

 FOpenSessionData()
 {
  Operation = OpenSessionOP;
 }

 void InitForEmail(const FString& ChallengeIn, const FString& CodeIn, const FString& SessionIdIn, const FString& VerifierIn, const bool ForceCreateAccountIn)
 {
  //Get Keccak(Challenge + Code)
  const FHash256 AnswerHash = FHash256::New();
  const FUnsizedData EncodedAnswerData = StringToUTF8(ChallengeIn + CodeIn);
  Keccak256::getHash(EncodedAnswerData.Arr.Get()->GetData(), EncodedAnswerData.GetLength(), AnswerHash.Ptr());
  answer = "0x" + AnswerHash.ToHex();
  
  forceCreateAccount = ForceCreateAccountIn;
  identityType = EmailType;
  sessionId = SessionIdIn;
  verifier = VerifierIn;
 }

 void InitForGuest(const FString& ChallengeIn, const FString& SessionIdIn, const bool ForceCreateAccountIn)
 {
  //Get Keccak(Challenge + SessionId)
  const FHash256 AnswerHash = FHash256::New();
  const FUnsizedData EncodedAnswerData = StringToUTF8(ChallengeIn + SessionIdIn);
  Keccak256::getHash(EncodedAnswerData.Arr.Get()->GetData(), EncodedAnswerData.GetLength(), AnswerHash.Ptr());
  answer = "0x" + AnswerHash.ToHex();

  forceCreateAccount = ForceCreateAccountIn;
  identityType = GuestType;
  sessionId = SessionIdIn;
  verifier = SessionIdIn;
 }
 
 void InitForOIDC(const FString& IdTokenIn, const FString& SessionIdIn, const bool ForceCreateAccountIn)
 {
  //Get Keccak(IdToken)
  const FHash256 PreTokenHash = FHash256::New();
  const FUnsizedData EncodedTokenData = StringToUTF8(IdTokenIn);
  Keccak256::getHash(EncodedTokenData.Ptr(), EncodedTokenData.GetLength(), PreTokenHash.Ptr());
  const FString IdTokenHash = "0x" + PreTokenHash.ToHex();

  forceCreateAccount = ForceCreateAccountIn;
  answer = IdTokenIn;
  identityType = OIDCType;
  sessionId = SessionIdIn;
  verifier = IdTokenHash + ";" + FString::Printf(TEXT("%lld"),USequenceSupport::GetInt64FromToken(IdTokenIn, "exp"));
 }

 void InitForPlayFab(const FString& SessionTicketIn, const FString& SessionIdIn, const bool ForceCreateAccountIn)
 {
  //Get Keccak(SessionTicketIn)
  const FHash256 PreTicketHash = FHash256::New();
  const FUnsizedData EncodedTicketData = StringToUTF8(SessionTicketIn);
  Keccak256::getHash(EncodedTicketData.Arr.Get()->GetData(), EncodedTicketData.GetLength(), PreTicketHash.Ptr());
  const FString TicketHash = "0x" + PreTicketHash.ToHex();

  forceCreateAccount = ForceCreateAccountIn;
  answer = SessionTicketIn;
  identityType = PlayFabType;
  sessionId = SessionIdIn;
  verifier = UConfigFetcher::GetConfigVar(UConfigFetcher::PlayFabTitleID) + "|" + TicketHash;
 }

 virtual FString GetJson() const override
 {
  return "";
 }
};

USTRUCT()
struct SEQUENCEPLUGIN_API FInitiateAuthData : public FGenericData
{
 GENERATED_USTRUCT_BODY()
 UPROPERTY()
 FString identityType = "";
 UPROPERTY()
 FString sessionId = "";
 UPROPERTY()
 FString verifier = "";

 FInitiateAuthData()
 {
  Operation = InitiateAuthOP;
 }

 void InitForEmail(const FString& SessionIdIn, const FString& EmailIn)
 {
  sessionId = SessionIdIn;
  identityType = EmailType;
  verifier = EmailIn + ";" + SessionIdIn;
 }

 void InitForGuest(const FString& SessionIdIn)
 {
  sessionId = SessionIdIn;
  identityType = GuestType;
  verifier = SessionIdIn;
 }

 void InitForOIDC(const FString& IdTokenIn, const FString& SessionIdIn)
 {
  const FHash256 PreTokenHash = FHash256::New();
  const FUnsizedData EncodedTokenData = StringToUTF8(IdTokenIn);
  Keccak256::getHash(EncodedTokenData.Ptr(), EncodedTokenData.GetLength(), PreTokenHash.Ptr());
  const FString IdTokenHash = "0x" + PreTokenHash.ToHex();
  
  identityType = OIDCType;
  sessionId = SessionIdIn;
  verifier = IdTokenHash + ";" + FString::Printf(TEXT("%lld"),USequenceSupport::GetInt64FromToken(IdTokenIn, "exp"));
 }

 void InitForPlayFab(const FString& SessionTicketIn, const FString& SessionIdIn)
 {
  const FHash256 PreTicketHash = FHash256::New();
  const FUnsizedData EncodedTicketData = StringToUTF8(SessionTicketIn);
  Keccak256::getHash(EncodedTicketData.Ptr(), EncodedTicketData.GetLength(), PreTicketHash.Ptr());
  const FString TicketHash = "0x" + PreTicketHash.ToHex();
  
  identityType = PlayFabType;
  sessionId = SessionIdIn;
  verifier = UConfigFetcher::GetConfigVar(UConfigFetcher::PlayFabTitleID) + "|" + TicketHash;
 }

 virtual FString GetJson() const override
 {
  return "";
 }
};

USTRUCT()
struct SEQUENCEPLUGIN_API FFederateAccountData : public FGenericData
{
 GENERATED_USTRUCT_BODY()

 UPROPERTY()
 FString answer = "";
 UPROPERTY()
 FString identityType = "";
 UPROPERTY()
 FString sessionId = "";
 UPROPERTY()
 FString verifier = "";
 UPROPERTY()
 FString wallet = "";

 FFederateAccountData()
 {
  Operation = FederateSessionOP;
 }
 
 void InitForEmail(const FString& WalletIn, const FString& ChallengeIn, const FString& CodeIn, const FString& SessionIdIn, const FString& VerifierIn)
 {
  //Get Keccak(Challenge + Code)
  const FHash256 AnswerHash = FHash256::New();
  const FUnsizedData EncodedAnswerData = StringToUTF8(ChallengeIn + CodeIn);
  Keccak256::getHash(EncodedAnswerData.Arr.Get()->GetData(), EncodedAnswerData.GetLength(), AnswerHash.Ptr());
  answer = "0x" + AnswerHash.ToHex();
  wallet = WalletIn;
  identityType = EmailType;
  sessionId = SessionIdIn;
  verifier = VerifierIn;
 }

 void InitForFederation(const FOpenSessionData& OpenSessionDataIn, const FString& WalletIn)
 {
  answer = OpenSessionDataIn.answer;
  wallet = WalletIn;
  identityType = OpenSessionDataIn.identityType;
  sessionId = OpenSessionDataIn.sessionId;
  verifier = OpenSessionDataIn.verifier;
 }
 
 void InitForOIDC(const FString& WalletIn, const FString& IdTokenIn, const FString& SessionIdIn)
 {
  //Get Keccak(IdToken)
  const FHash256 PreTokenHash = FHash256::New();
  const FUnsizedData EncodedTokenData = StringToUTF8(IdTokenIn);
  Keccak256::getHash(EncodedTokenData.Ptr(), EncodedTokenData.GetLength(), PreTokenHash.Ptr());
  const FString IdTokenHash = "0x" + PreTokenHash.ToHex();
  wallet = WalletIn;
  answer = IdTokenIn;
  identityType = OIDCType;
  sessionId = SessionIdIn;
  verifier = IdTokenHash + ";" + FString::Printf(TEXT("%lld"),USequenceSupport::GetInt64FromToken(IdTokenIn, "exp"));
 }

 void InitForPlayFab(const FString& WalletIn, const FString& SessionTicketIn, const FString& SessionIdIn)
 {
  //Get Keccak(SessionTicketIn)
  const FHash256 PreTicketHash = FHash256::New();
  const FUnsizedData EncodedTicketData = StringToUTF8(SessionTicketIn);
  Keccak256::getHash(EncodedTicketData.Arr.Get()->GetData(), EncodedTicketData.GetLength(), PreTicketHash.Ptr());
  const FString TicketHash = "0x" + PreTicketHash.ToHex();

  wallet = WalletIn;
  answer = SessionTicketIn;
  identityType = PlayFabType;
  sessionId = SessionIdIn;
  verifier = UConfigFetcher::GetConfigVar(UConfigFetcher::PlayFabTitleID) + "|" + TicketHash;
 }

 virtual FString GetJson() const override
 {
  return "";
 }
};

USTRUCT()
struct SEQUENCEPLUGIN_API FCloseSessionData : public FGenericData
{
 GENERATED_USTRUCT_BODY()
 UPROPERTY()
 FString sessionId = "";

 FCloseSessionData()
 {
  Operation = CloseSessionOP;
 }
 
 FCloseSessionData(const FString& SessionIdIn)
 {
  Operation = CloseSessionOP;
  sessionId = SessionIdIn;
 }

 virtual FString GetJson() const override
 {
  return "";
 }
};

struct SEQUENCEPLUGIN_API FGetFeeOptionsData : public FGenericData
{
 FString network = "";
 TArray<TransactionUnion> transactions;
 FString wallet = "";

 FGetFeeOptionsData()
 {
  UseCustomParser = true;
  Operation = FeeOptionsOP;
 }
 
 FGetFeeOptionsData(const FString& NetworkIn, const TArray<TransactionUnion>& TransactionsIn, const FString& WalletIn)
 {
  UseCustomParser = true;
  Operation = FeeOptionsOP;
  network = NetworkIn;
  transactions = TransactionsIn;
  wallet = WalletIn;
 }

 virtual FString GetJson() const override
 {
  const FString Json = "{\"network\":\""+network+"\",\"transactions\":"+USequenceSupport::TransactionListToJsonString(transactions)+",\"wallet\":\""+wallet+"\"}";
  return Json;
 }
};

struct SEQUENCEPLUGIN_API FSendTransactionData : public FGenericData
{
 FString identifier = "";
 FString network = "";
 TArray<TransactionUnion> transactions;
 FString wallet = "";

 FSendTransactionData()
 {
  UseCustomParser = true;
  Operation = SendTransactionOP;
 }
 
 FSendTransactionData(const FString& IdentifierIn, const FString& NetworkIn, const TArray<TransactionUnion>& TransactionsIn, const FString& WalletIn)
 {
  UseCustomParser = true;
  Operation = SendTransactionOP;
  identifier = IdentifierIn;
  network = NetworkIn;
  transactions = TransactionsIn;
  wallet = WalletIn;
 }

 virtual FString GetJson() const override
 {
  const FString Json = "{\"identifier\":\""+identifier+"\",\"network\":\""+network+"\",\"transactions\":"+USequenceSupport::TransactionListToJsonString(transactions)+",\"wallet\":\""+wallet+"\"}";
  return Json;
 }
};

struct SEQUENCEPLUGIN_API FSendTransactionWithFeeOptionData : public FGenericData
{
 FString identifier = "";
 FString network = "";
 TArray<TransactionUnion> transactions;
 FString transactionsFeeQuote = "";
 FString wallet = "";

 FSendTransactionWithFeeOptionData()
 {
  UseCustomParser = true;
  Operation = SendTransactionOP;
 }
 
 FSendTransactionWithFeeOptionData(const FString& IdentifierIn, const FString& NetworkIn, const TArray<TransactionUnion>& TransactionsIn,const FString& TransactionsFeeQuoteIn, const FString& WalletIn)
 {
  UseCustomParser = true;
  Operation = SendTransactionOP;
  identifier = IdentifierIn;
  network = NetworkIn;
  transactions = TransactionsIn;
  transactionsFeeQuote = TransactionsFeeQuoteIn;
  wallet = WalletIn;
 }

 virtual FString GetJson() const override
 {
  const FString Json = "{\"identifier\":\""+identifier+"\",\"network\":\""+network+"\",\"transactions\":"+USequenceSupport::TransactionListToJsonString(transactions)+",\"transactionsFeeQuote\":\""+transactionsFeeQuote+"\",\"wallet\":\""+wallet+"\"}";
  return Json;
 }
};

USTRUCT()
struct SEQUENCEPLUGIN_API FListSessionsData : public FGenericData
{
 GENERATED_USTRUCT_BODY()
 UPROPERTY()
 FString wallet = "";

 FListSessionsData()
 {
  Operation = ListSessionsOP;
 }
 
 FListSessionsData(const FString& WalletIn)
 {
  Operation = ListSessionsOP;
  wallet = WalletIn;
 }

 virtual FString GetJson() const override
 {
  return "";
 }
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSignMessageData : public FGenericData
{
 GENERATED_USTRUCT_BODY()
 UPROPERTY()
 FString message = "";
 UPROPERTY()
 FString network = "";
 UPROPERTY()
 FString wallet = "";

 FSignMessageData()
 {
  Operation = SignMessageOP;
 }
 
 FSignMessageData(const FString& MessageIn, const FString& NetworkIn, const FString& WalletIn)
 {
  Operation = SignMessageOP;
  message = MessageIn;
  network = NetworkIn;
  wallet = WalletIn;
 }

 virtual FString GetJson() const override
 {
  return "";
 }
};

struct SEQUENCEPLUGIN_API FSignatureIntent
{ 
 FGenericData * data = nullptr;
 int64 expiresAt = 0;
 int64 issuedAt = 0;
 FString name = "";
 FString version = "";

 FSignatureIntent(){}
 
 FSignatureIntent(FGenericData * DataIn, int64 ExpiresAtIn, int64 IssuedAtIn, const FString& NameIn, const FString& VersionIn)
 {
  data = DataIn;
  expiresAt = ExpiresAtIn;
  issuedAt = IssuedAtIn;
  name = NameIn;
  version = VersionIn;
 }
 
 template<typename T> FString GetJson() const
 {
  T CData = *static_cast<T*>(data);
  const FString DataJson = (CData.UseCustomParser)? CData.GetJson() : USequenceSupport::StructToPartialSimpleString<T>(CData);
  const FString IssuedString = FString::Printf(TEXT("%lld"),issuedAt);
  const FString ExpiresString = FString::Printf(TEXT("%lld"),expiresAt);
  const FString Json = "{\"data\":" + DataJson + ",\"expiresAt\":" + ExpiresString + ",\"issuedAt\":" + IssuedString + ",\"name\":\""+name+"\",\"version\":\""+version+"\"}";
  return Json;
 }
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSignatureEntry
{
 GENERATED_USTRUCT_BODY()
 UPROPERTY()
 FString sessionId = "";
 UPROPERTY()
 FString signature = "";

 FSignatureEntry(){}
 
 FSignatureEntry(const FString& SessionIdIn, const FString& SignatureIn)
 {
  sessionId = SessionIdIn;
  signature = SignatureIn;
 }
};

struct SEQUENCEPLUGIN_API FSignedIntent
{
 FGenericData * data = nullptr;
 int64 expiresAt = 0;
 int64 issuedAt = 0;
 FString name = "";
 TArray<FSignatureEntry> signatures;
 FString version = "";

 FSignedIntent(){}
 
 FSignedIntent(FGenericData * DataIn, int64 ExpiresAtIn, int64 IssuedAtIn, const FString& Operation, const TArray<FSignatureEntry>& SignaturesIn, const FString& VersionIn)
 {
  data = DataIn;
  expiresAt = ExpiresAtIn;
  issuedAt = IssuedAtIn;
  name = Operation;
  signatures = SignaturesIn;
  version = VersionIn;
 }

 template <typename T> FString GetJson() const
 {
  T CData = *static_cast<T*>(data);
  const FString DataJson = (CData.UseCustomParser)? CData.GetJson() : USequenceSupport::StructToPartialSimpleString<T>(CData);
  const FString IssuedString = FString::Printf(TEXT("%lld"),issuedAt);
  const FString ExpiresString = FString::Printf(TEXT("%lld"),expiresAt);
  FString SigListJson = "[";
  for (int i = 0; i < signatures.Num(); i++)
  {
   SigListJson += USequenceSupport::StructToPartialSimpleString(signatures[i]);
   if (i+1 < signatures.Num())
   {
    SigListJson += ",";
   }
  }
  SigListJson += "]";

  const FString Json = "{\"data\":"+DataJson+",\"expiresAt\":"+ExpiresString+",\"issuedAt\":"+IssuedString+",\"name\":\""+name+"\",\"signatures\":"+SigListJson+",\"version\":\""+version+"\"}";
  return Json;
 }
};

struct SEQUENCEPLUGIN_API FGenericFinalIntent
{
 FSignedIntent intent;

 FGenericFinalIntent(){}
 
 FGenericFinalIntent(const FSignedIntent& IntentIn)
 {
  intent = IntentIn;
 }

 template <typename T> FString GetJson() const
 {
  const FString IntentJson = intent.GetJson<T>();
  const FString Json = "{\"intent\":"+IntentJson+"}";
  return Json;
 }
};

struct SEQUENCEPLUGIN_API FRegisterFinalIntent
{
 FSignedIntent intent;
 FString friendlyName = "";

 FRegisterFinalIntent(){}

 FRegisterFinalIntent(const FSignedIntent& IntentIn, const FString& FriendlyName)
 {
  intent = IntentIn;
  friendlyName = FriendlyName;
 }

 template <typename T> FString GetJson() const
 {
  const FString IntentJson = intent.GetJson<T>();
  const FString Json = "{\"intent\":"+IntentJson+",\"friendlyName\":\""+friendlyName+"\"}";
  return Json;
 }
};