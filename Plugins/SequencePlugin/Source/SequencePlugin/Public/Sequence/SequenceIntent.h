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

/*

registerSession cleanup checks (passed)

Old: {"intent":{"data":{"idToken":"","sessionId":"0x00dd64dc6917182b8f3a259907cfcb1156519cce84"},"expiresAt":1718394377,"issuedAt":1718307977,"name":"openSession","signatures":[{"sessionId":"0x00dd64dc6917182b8f3a259907cfcb1156519cce84","signature":"0xf165fcf6346449f32be0223755ee589366b530839c3a507ffd41efd118350ad80baa3dbe5050d2b87f6bc67547120a4fc47f39d0484523e483dfdb7adae7aa9400"}],"version":"1.0.0"},"friendlyName":"BA03872142162E589437DF976A82F91C"}
New: {"intent":{"data":{"idToken":"","sessionId":"0x00dd64dc6917182b8f3a259907cfcb1156519cce84"},"expiresAt":1718394377,"issuedAt":1718307977,"name":"openSession","signatures":[{"sessionId":"0x00dd64dc6917182b8f3a259907cfcb1156519cce84","signature":"0xf165fcf6346449f32be0223755ee589366b530839c3a507ffd41efd118350ad80baa3dbe5050d2b87f6bc67547120a4fc47f39d0484523e483dfdb7adae7aa9400"}],"version":"1.0.0"},"friendlyName":"CCCD39EA4E809E735C3AC19598B9A355"}

ListSessions cleanup checks (passed)

Old: {"intent":{"data":{"wallet":"0x5Fb06bF3549F4823A983Cf60bab3d3120A7152a5"},"expiresAt":1718394503,"issuedAt":1718308103,"name":"listSessions","signatures":[{"sessionId":"0x00dd64dc6917182b8f3a259907cfcb1156519cce84","signature":"0x631c6abac81cedf37817fed8e39d26de6055ecda4288dfb44a5d596b904955315bd802e0c623b238b86d98b455f5367a84daaf9433842a0de3a6a3c10da03dd301"}],"version":"1.0.0"}}
New: {"intent":{"data":{"wallet":"0x5Fb06bF3549F4823A983Cf60bab3d3120A7152a5"},"expiresAt":1718394503,"issuedAt":1718308103,"name":"listSessions","signatures":[{"sessionId":"0x00dd64dc6917182b8f3a259907cfcb1156519cce84","signature":"0x631c6abac81cedf37817fed8e39d26de6055ecda4288dfb44a5d596b904955315bd802e0c623b238b86d98b455f5367a84daaf9433842a0de3a6a3c10da03dd301"}],"version":"1.0.0"}}

SignMessage cleanup checks (passed)

Old: {"intent":{"data":{"message":"0x19457468657265756D205369676E6564204D6573736167653A0A326869","network":"137","wallet":"0x5Fb06bF3549F4823A983Cf60bab3d3120A7152a5"},"expiresAt":1718394603,"issuedAt":1718308203,"name":"signMessage","signatures":[{"sessionId":"0x00dd64dc6917182b8f3a259907cfcb1156519cce84","signature":"0xf8e3e2f41bb5211053321da4db9aef02ecb30dab2cde5b3c23bf7b50340bc01f4de4803064ce4b21f98ed79c6fad99fed81e988d9768a8b6a365563b5cefd33000"}],"version":"1.0.0"}}
New: {"intent":{"data":{"message":"0x19457468657265756D205369676E6564204D6573736167653A0A326869","network":"137","wallet":"0x5Fb06bF3549F4823A983Cf60bab3d3120A7152a5"},"expiresAt":1718394603,"issuedAt":1718308203,"name":"signMessage","signatures":[{"sessionId":"0x00dd64dc6917182b8f3a259907cfcb1156519cce84","signature":"0xf8e3e2f41bb5211053321da4db9aef02ecb30dab2cde5b3c23bf7b50340bc01f4de4803064ce4b21f98ed79c6fad99fed81e988d9768a8b6a365563b5cefd33000"}],"version":"1.0.0"}}

closeSession cleanup checks (passed)

Old: {"intent":{"data":{"sessionId":"0x00dd64dc6917182b8f3a259907cfcb1156519cce84"},"expiresAt":1718394688,"issuedAt":1718308288,"name":"closeSession","signatures":[{"sessionId":"0x00dd64dc6917182b8f3a259907cfcb1156519cce84","signature":"0x5acd6adc9905372d6db32c9ceeed764563151ada74b438c069b60da7b00d417a5a51d72c4c1c3d0e9530451bea7201084dd3db39fe21a9b3eefa6a00d1424c9100"}],"version":"1.0.0"}}
New: {"intent":{"data":{"sessionId":"0x00dd64dc6917182b8f3a259907cfcb1156519cce84"},"expiresAt":1718394688,"issuedAt":1718308288,"name":"closeSession","signatures":[{"sessionId":"0x00dd64dc6917182b8f3a259907cfcb1156519cce84","signature":"0x5acd6adc9905372d6db32c9ceeed764563151ada74b438c069b60da7b00d417a5a51d72c4c1c3d0e9530451bea7201084dd3db39fe21a9b3eefa6a00d1424c9100"}],"version":"1.0.0"}}

sendTransaction cleanup checks (passed)

Old: {"intent":{"data":{"identifier":"unreal-sdk-2024.06.14-17.51.24-0x5Fb06bF3549F4823A983Cf60bab3d3120A7152a5","network":"137","transactions":[{"to":"","tokenAddress":"","type":"erc20send","value":"1000"},{"to":"","tokenAddress":"","type":"erc20send","value":"1000"},{"to":"","tokenAddress":"","type":"erc20send","value":"1000"},{"to":"","tokenAddress":"","type":"erc20send","value":"1000"}],"wallet":"0x5Fb06bF3549F4823A983Cf60bab3d3120A7152a5"},"expiresAt":1718473854,"issuedAt":1718387454,"name":"sendTransaction","signatures":[{"sessionId":"0x008755da90a23a385bed05f921fcc5d369a3d49abd","signature":"0xbfb8a994fd057458e3bfea0a6351e20fe068ba8dc4fc51e24f091b23cafb9e3d3142c5d5954615018404d213fa27c3f5327e1cd524a107bcbbabcfabec11231500"}],"version":"1.0.0"}}
New: {"intent":{"data":{"identifier":"unreal-sdk-2024.06.14-17.51.24-0x5Fb06bF3549F4823A983Cf60bab3d3120A7152a5","network":"137","transactions":[{"to":"","tokenAddress":"","type":"erc20send","value":"1000"},{"to":"","tokenAddress":"","type":"erc20send","value":"1000"},{"to":"","tokenAddress":"","type":"erc20send","value":"1000"},{"to":"","tokenAddress":"","type":"erc20send","value":"1000"}],"wallet":"0x5Fb06bF3549F4823A983Cf60bab3d3120A7152a5"},"expiresAt":1718473854,"issuedAt":1718387454,"name":"sendTransaction","signatures":[{"sessionId":"0x008755da90a23a385bed05f921fcc5d369a3d49abd","signature":"0xbfb8a994fd057458e3bfea0a6351e20fe068ba8dc4fc51e24f091b23cafb9e3d3142c5d5954615018404d213fa27c3f5327e1cd524a107bcbbabcfabec11231500"}],"version":"1.0.0"}}

getTransactionFeeOptions cleanup checks (passed)

Old: {"intent":{"data":{"network":"137","transactions":[{"to":"","tokenAddress":"","type":"erc20send","value":"1000"},{"to":"","tokenAddress":"","type":"erc20send","value":"1000"},{"to":"","tokenAddress":"","type":"erc20send","value":"1000"},{"to":"","tokenAddress":"","type":"erc20send","value":"1000"}],"wallet":"0x5Fb06bF3549F4823A983Cf60bab3d3120A7152a5"},"expiresAt":1718473377,"issuedAt":1718386977,"name":"feeOptions","signatures":[{"sessionId":"0x008755da90a23a385bed05f921fcc5d369a3d49abd","signature":"0x4dab9093c1b7043a0b79207394b7ef46d1383aa620ccc71177889b3b1b906e85100b94426188e430e2d962368e076e026390f02075b47f4aaf74e136744f721100"}],"version":"1.0.0"}}
New: {"intent":{"data":{"network":"137","transactions":[{"to":"","tokenAddress":"","type":"erc20send","value":"1000"},{"to":"","tokenAddress":"","type":"erc20send","value":"1000"},{"to":"","tokenAddress":"","type":"erc20send","value":"1000"},{"to":"","tokenAddress":"","type":"erc20send","value":"1000"}],"wallet":"0x5Fb06bF3549F4823A983Cf60bab3d3120A7152a5"},"expiresAt":1718473377,"issuedAt":1718386977,"name":"feeOptions","signatures":[{"sessionId":"0x008755da90a23a385bed05f921fcc5d369a3d49abd","signature":"0x4dab9093c1b7043a0b79207394b7ef46d1383aa620ccc71177889b3b1b906e85100b94426188e430e2d962368e076e026390f02075b47f4aaf74e136744f721100"}],"version":"1.0.0"}}

sendTransactionWithFeeOption cleanup checks (passed)

Old: {"intent":{"data":{"identifier":"","network":"137","transactions":[{"to":"","tokenAddress":"","type":"erc20send","value":"3497"},{"to":"","tokenAddress":"","type":"erc20send","value":"10"}],"transactionsFeeQuote":"","wallet":"0x5Fb06bF3549F4823A983Cf60bab3d3120A7152a5"},"expiresAt":1718475935,"issuedAt":1718389535,"name":"sendTransaction","signatures":[{"sessionId":"0x00efe9a3edf48da21f115a4bc04b8751ffaa6b903f","signature":"0xac81f24114da27fba7d219ce14725e8493a32ac4e08090047948b8b71d9cc0fa18a2811a70bd3c089d80417c96018aabae8f11ac957af23f2cf70136d881dc1a01"}],"version":"1.0.0"}}
New: {"intent":{"data":{"identifier":"","network":"137","transactions":[{"to":"","tokenAddress":"","type":"erc20send","value":"3497"},{"to":"","tokenAddress":"","type":"erc20send","value":"10"}],"transactionsFeeQuote":"","wallet":"0x5Fb06bF3549F4823A983Cf60bab3d3120A7152a5"},"expiresAt":1718475935,"issuedAt":1718389535,"name":"sendTransaction","signatures":[{"sessionId":"0x00efe9a3edf48da21f115a4bc04b8751ffaa6b903f","signature":"0xac81f24114da27fba7d219ce14725e8493a32ac4e08090047948b8b71d9cc0fa18a2811a70bd3c089d80417c96018aabae8f11ac957af23f2cf70136d881dc1a01"}],"version":"1.0.0"}}

*/

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

USTRUCT()
struct FGetFeeOptionsData : public FGenericData
{
 GENERATED_USTRUCT_BODY()
 UPROPERTY()
 FString network = "";
 //UPROPERTY()
 TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> transactions;
 UPROPERTY()
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

USTRUCT()
struct FSendTransactionData : public FGenericData
{
 GENERATED_USTRUCT_BODY()
 UPROPERTY()
 FString identifier = "";
 UPROPERTY()
 FString network = "";
 //UPROPERTY()
 TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> transactions;
 UPROPERTY()
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

USTRUCT()
struct FSendTransactionWithFeeOptionData : public FGenericData
{
 GENERATED_USTRUCT_BODY()
 UPROPERTY()
 FString identifier = "";
 UPROPERTY()
 FString network = "";
 //UPROPERTY()
 TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> transactions;
 UPROPERTY()
 FString transactionsFeeQuote = "";
 UPROPERTY()
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
  UE_LOG(LogTemp, Display, TEXT("Parsed Json: %s"),*Json);
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