// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Containers/Union.h"
#include "Indexer/IndexerSupport.h"
#include "Util/Structs/BE_Structs.h"
#include "SequenceIntent.generated.h"

//Operations Constants//

static const FString OpenSessionOP = "openSession";
static const FString CloseSessionOP = "closeSession";
static const FString SendTransactionOP = "sendTransaction";
static const FString FeeOptionsOP = "feeOptions";
static const FString ListSessionsOP = "listSessions";
static const FString SignMessageOP = "signMessage";

//Operations Constants//

USTRUCT()
struct FGenericData
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
struct FCloseSessionData : public FGenericData
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

struct FGetFeeOptionsData : public FGenericData
{
 FString network = "";
 TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> transactions;
 FString wallet = "";

 FGetFeeOptionsData()
 {
  UseCustomParser = true;
  Operation = FeeOptionsOP;
 }
 
 FGetFeeOptionsData(const FString& NetworkIn, const TArray<TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>>& TransactionsIn, const FString& WalletIn)
 {
  UseCustomParser = true;
  Operation = FeeOptionsOP;
  network = NetworkIn;
  transactions = TransactionsIn;
  wallet = WalletIn;
 }

 virtual FString GetJson() const override
 {
  const FString Json = "{\"network\":\""+network+"\",\"transactions\":"+UIndexerSupport::TransactionListToJsonString(transactions)+",\"wallet\":\""+wallet+"\"}";
  return Json;
 }
};

struct FSendTransactionData : public FGenericData
{
 FString identifier = "";
 FString network = "";
 TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> transactions;
 FString wallet = "";

 FSendTransactionData()
 {
  UseCustomParser = true;
  Operation = SendTransactionOP;
 }
 
 FSendTransactionData(const FString& IdentifierIn, const FString& NetworkIn, const TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>>& TransactionsIn, const FString& WalletIn)
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
  const FString Json = "{\"identifier\":\""+identifier+"\",\"network\":\""+network+"\",\"transactions\":"+UIndexerSupport::TransactionListToJsonString(transactions)+",\"wallet\":\""+wallet+"\"}";
  return Json;
 }
};

struct FSendTransactionWithFeeOptionData : public FGenericData
{
 FString identifier = "";
 FString network = "";
 TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> transactions;
 FString transactionsFeeQuote = "";
 FString wallet = "";

 FSendTransactionWithFeeOptionData()
 {
  UseCustomParser = true;
  Operation = SendTransactionOP;
 }
 
 FSendTransactionWithFeeOptionData(const FString& IdentifierIn, const FString& NetworkIn, const TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>>& TransactionsIn,const FString& TransactionsFeeQuoteIn, const FString& WalletIn)
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
  const FString Json = "{\"identifier\":\""+identifier+"\",\"network\":\""+network+"\",\"transactions\":"+UIndexerSupport::TransactionListToJsonString(transactions)+",\"transactionsFeeQuote\":\""+transactionsFeeQuote+"\",\"wallet\":\""+wallet+"\"}";
  return Json;
 }
};

USTRUCT()
struct FRegisterSessionData : public FGenericData
{
 GENERATED_USTRUCT_BODY()
 UPROPERTY()
 FString idToken = "";
 UPROPERTY()
 FString sessionId = "";

 FRegisterSessionData()
 {
  Operation = OpenSessionOP;
 }
 
 FRegisterSessionData(const FString& IdTokenIn, const FString& SessionIdIn)
 {
  Operation = OpenSessionOP;
  idToken = IdTokenIn;
  sessionId = SessionIdIn;
 }

 virtual FString GetJson() const override
 {
  return "";
 }
};

USTRUCT()
struct FListSessionsData : public FGenericData
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
struct FSignMessageData : public FGenericData
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

struct FSignatureIntent
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
  const FString DataJson = (CData.UseCustomParser)? CData.GetJson() : UIndexerSupport::StructToPartialSimpleString<T>(CData);
  const FString IssuedString = FString::Printf(TEXT("%lld"),issuedAt);
  const FString ExpiresString = FString::Printf(TEXT("%lld"),expiresAt);
  const FString Json = "{\"data\":" + DataJson + ",\"expiresAt\":" + ExpiresString + ",\"issuedAt\":" + IssuedString + ",\"name\":\""+name+"\",\"version\":\""+version+"\"}";
  return Json;
 }
};

USTRUCT()
struct FSignatureEntry
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

struct FSignedIntent
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
  const FString DataJson = (CData.UseCustomParser)? CData.GetJson() : UIndexerSupport::StructToPartialSimpleString<T>(CData);
  const FString IssuedString = FString::Printf(TEXT("%lld"),issuedAt);
  const FString ExpiresString = FString::Printf(TEXT("%lld"),expiresAt);
  FString SigListJson = "[";
  for (int i = 0; i < signatures.Num(); i++)
  {
   SigListJson += UIndexerSupport::StructToPartialSimpleString(signatures[i]);
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

struct FGenericFinalIntent
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

struct FRegisterFinalIntent
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
// test function signature
// FString GenerateIntent(const FString& Data, const FString& Operation)

/*
 * Data portion of FeeOptions Intent
 {"network":"","transactions":[],"wallet":""}
 * Intent portion of FeeOptions Intent
 {"data":{},"expiresAt":,"issuedAt":,"name":"feeOptions","version":""}
*/

/*
 * Data
 {"network":"","transactions":[],"wallet":""}
 * FeeOptions final intent
 {"intent":{"data":{},"expiresAt":,"issuedAt":,"name":"feeOptions","signatures":[{"sessionId":"","signature":""}],"version":""}}
*/

/*
 * Data portion of SendTransaction intent
 {"identifier":"","network":"","transactions":[],"wallet":""}
 * Intent portion of SendTransaction intent
 {"data":{},"expiresAt":,"issuedAt":,"name":"sendTransaction","version":""}
*/

/*
 * Data
 {"identifier":"","network":"","transactions":[],"wallet":""}
 * Send transaction final intent
 {"intent":{"data":{},"expiresAt":"","issuedAt":"","name":"sendTransaction","signatures":[{"sessionId":"","signature":""}],"version":""}}
*/

/*
 * Data portion of SendTransactionWithFeeOption intent
 {"identifier":"","network":"","transactions":"","transactionsFeeQuote":"","wallet":""}
 * Intent portion of SendTransaction intent
 {"data":{},"expiresAt":"","issuedAt":"","name":"sendTransaction","version":""}
*/

/*
 * Data
 {"identifier":"","network":"","transactions":[],"transactionsFeeQuote":"","wallet":""}
 * SendTransactionWithFee final intent
 {"intent":{"data":{},"expiresAt":,"issuedAt":,"name":"sendTransaction","signatures":[{"sessionId":"","signature":""}],"version":""}}
*/

/*
 * Data portion of SignMessage Intent
 {"message":"","network":"","wallet":""}
 * Intent portion of SignMessage Intent
 {"data":{},"expiresAt":"","issuedAt":"","name":"signMessage","version":""}
*/

/*
 * Data
 {"message":"","network":"","wallet":""}
 * SignMessage final intent
 {"intent":{"data":{},"expiresAt":,"issuedAt":,"name":"signMessage","signatures":[{"sessionId":"","signature":""}],"version":""}}
*/

/*
 * Data portion of register session intent
 {"idToken":"","sessionId":""}
 * Intent portion of register session intent
 {"data":{},"expiresAt":"","issuedAt":"","name":"openSession","version":""}
*/

/*
 * Data
 {"idToken":"","sessionId":""}
 * Register session final intent
 {"intent":{"data":{},"expiresAt":,"issuedAt":,"name":"openSession","signatures":[{"sessionId":"","signature":""}],"version":""},"friendlyName":""}
*/

/*
 * Data portion of list session intent
 {"wallet":""}
 * Intent portion of list session intent
 {"data":{},"expiresAt":"","issuedAt":"","name":"listSessions","version":""}
*/

/*
 * Data
 {"wallet":""}
 * List session final intent
 {"intent":{"data":{},"expiresAt":,"issuedAt":,"name":"listSessions","signatures":[{"sessionId":"","signature":""}],"version":""}}
*/

/*
 * Data portion of Close session intent
 {"sessionId":""}
 * Intent portion of CLose Session Intent
 {"data":{},"expiresAt":"","issuedAt":"","name":"closeSession","version":""}
*/

/*
 * Data
 {"sessionId":""}
 * Close session final intent
 {"intent":{"data":{},"expiresAt":,"issuedAt":,"name":"closeSession","signatures":[{"sessionId":"","signature":""}],"version":""}}
*/

/*

{"intent":{"data":{},"expiresAt":,"issuedAt":,"name":"","signatures":[{"sessionId":"","signature":""}],"version":""}}
{"intent":{"data":{},"expiresAt":,"issuedAt":,"name":"","signatures":[{"sessionId":"","signature":""}],"version":""}}
{"intent":{"data":{},"expiresAt":,"issuedAt":,"name":"","signatures":[{"sessionId":"","signature":""}],"version":""}}
{"intent":{"data":{},"expiresAt":,"issuedAt":,"name":"","signatures":[{"sessionId":"","signature":""}],"version":""},"friendlyName":""}
{"intent":{"data":{},"expiresAt":,"issuedAt":,"name":"","signatures":[{"sessionId":"","signature":""}],"version":""}}
{"intent":{"data":{},"expiresAt":,"issuedAt":,"name":"","signatures":[{"sessionId":"","signature":""}],"version":""}}
{"intent":{"data":{},"expiresAt":,"issuedAt":,"name":"","signatures":[{"sessionId":"","signature":""}],"version":""}}

*/