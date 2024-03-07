Sequence Unreal Plugin
======================

This plugin comes with everything needed to get connected and send transactions via web3.

Note: all files and folders talked about in this readme will be found in the following directories with in your project folder
/Plugins/SequencePlugin/Source/SequencePlugin/Private 
or
/Plugins/SequencePlugin/Source/SequencePlugin/Public

//Maybe add an excerpt about the use of the external ios plugin if we can't migrate in what we need for it

=========================================================================================================================

!!!BEFORE YOU CAN SAFELY USE THIS!!!
You must provide an encryption key implementation at [SequenceEncryptor.cpp] Function [GetStoredKey],
This function must be implemented to provide a securely stored private key that will be used to encrypt &
decrypt client information. Failure to do so will result in NO information being stored or in the event you
do not use a securely stored key, can result in client information being stored insecurely on their systems.
!!!BEFORE YOU CAN SAFELY USE THIS!!!

=========================================================================================================================
Getting Started

1) Once you have the the SequencePlugin folder & You're on a fresh project, you'll need to go to your project directory
and create a Plugins Folder in it, then copy over the Sequence Plugin folder into that Plugins folder, if a Plugins folder
already exists just copy this SequencePlugin folder into it.

2) Launch your project, once open allow it to update the UProject Settings.

3) To find the SequencePlugin content folder in your content drawer enable view plugin content

4) If you wish to use the in built sequence UI for login
4a) Create an [Actor] you wish to be responsible for the SequenceUI then attach the [Sequence_Pawn_Component_BP] to it
4b) Setup your [Actor] Blueprint similar to how it's setup in [Custom_Spectator_Pawn] being sure to bind to the delegate
that gives you Credentials [Auth_Success_Forwarder]

5) Once you have those credentials you'll need to forward them to your own C++ backend in order to use the SequenceAPI,
an example of this is with the [Custom_Spectator_Pawn] here this Pawn inherits from a C++ class [Sqnc_Spec_Pawn]
that class implements a blueprint Callable function [SetupCredentials(FCredentials_BE CredentialsIn)] which is callable
within the child class [Custom_Spectator_Pawn]. Calling this function will forward the credentials to a C++ backend.

6) before we get into using the rest of the SequenceAPI we'll cover how to handle the Authentication side of things first.

[IF you are using your own UI you'll need to do the following]

1) In a C++ backend with a series of pass through [UFUNCTIONS] setup similarly to [SequenceBackendManager.h/.cpp]
Each of these calls are implemented in [UAuthenticator] you just need to pass through the data with YOUR UAuthenticator UObject

//This call is platform dependent on windows & mac this is required for SSO WIP
   UFUNCTION(BlueprintCallable, CATEGORY = "Login")                      
   FString GetLoginURL(const ESocialSigninType& Type); 

//This Call is made after you've collected the ID_Token (Mac & Windows only) WIP
   UFUNCTION(BlueprintCallable, CATEGORY = "Login")
   void SocialLogin(const FString& IDTokenIn);

//This Call is made after you've collected the email address from the Users in the UI
   UFUNCTION(BlueprintCallable, CATEGORY = "Login")
   void EmailLogin(const FString& EmailIn);

//This is call is made after the Delegate [AuthRequiresCode] is fired
   UFUNCTION(BlueprintCallable, CATEGORY = "Login")
   void EmailCode(const FString& CodeIn);

//Optional call used to check if the credentials on disk are valid or not//
   UFUNCTION(BlueprintCallable, Category = "Login")
   bool StoredCredentialsValid();

To start you'll want to create a [UAuthenticator] UObject like so [UAuthenticator * Auth = NewObject<UAuthenticator>()],
this UObject manages the authentication side of Sequence.

2) Be sure to bind to the Delegates for [AuthSuccess], [AuthFailure], [AuthRequiresCode] prior to making any signin calls
You can bind to these delegates like so:
   this->authenticator->AuthRequiresCode.AddDynamic(this, &AYourClass::YourCallReadyToReceiveCode);
   this->authenticator->AuthFailure.AddDynamic(this, &AYourClass::YourCallShowAuthFailureScreen);

   In the case of [AuthSuccess] since a parameter is also passed we bind to it like this
   FScriptDelegate del;
   del.BindUFunction(this, "CallShowAuthSuccessScreen");
   this->authenticator->AuthSuccess.Add(del);
   
   Where [CallShowAuthSuccessScreen] is defined in SequenceBackendManager.h like so:

   UFUNCTION()
   void CallShowAuthSuccessScreen(const FCredentials_BE& CredentialsIn);

   & in SequenceBackendManager.cpp like so:

   void ASequenceBackendManager::CallShowAuthSuccessScreen(const FCredentials_BE& CredentialsIn) 
   {
   this->Credentials = CredentialsIn;
   if (this->ShowAuthSuccessDelegate.IsBound())
      this->ShowAuthSuccessDelegate.Broadcast(Credentials);
   else
      UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: ShowAuthSuccessDelegate]"));
   }

[Email based Authentication]
1) To start email based authentication you'll start it with this call [EmailLogin(const FString& EmailIn)], supplying
   an email you've collected from the User in your GUI.

2) Next [AuthRequiresCode] will fire when the backend is ready to receive the Code from your UI. Collect this code from
   your GUI and send it to the authenticator using [EmailCode(CodeIn)].

3) Finally [AuthSuccess] will fire with a Credentials_BE struct as a parameter. This is your non registered credentials
   from EmailAuth. You are done Email Based Auth.

[Social Signin based Authentication DESKTOP]
1) To start SSO based authentication with desktop you can either use your own implementation to get the necessary
   id_token or you can make use of Unreal's web browser plugin.

2) With whatever implementation you chose you can forward the colleted id_token to the UAuthenticator object with
   [SocialLogin(const FString& IDTokenIn)], after which [AuthSuccess] will fire and you're done desktop based SSO!

[Social Signin based Authentication MOBILE]
WIP

[SequenceAPI]
1) After you've completed initial authentication and have intercepted the credentials either through your UI or ours,
   to use the Sequence API you'll need to create a [USequenceWallet] by using:
   [USequenceWallet * Api = USequenceWallet(CredentialsIn)] or [USequenceWallet * Api = USequenceWallet::Make(CredentialsIn, ProviderURL)]
   Once you have your [USequenceWallet] UObject call [Api->RegisterSession(OnSuccess,GenericFailure)] this will register
   your credentials with the SequenceBackend. Note not calling Register prior to make any others calls will result in errors
   as a UserWallet hasn't been supplied until this point.

=========================================================================================================================

Assuming you've setup your controlling Actor with the [Sequence_Pawn_Component_BP]
The sequence pawn component has functions to do the following:

Setup Sequence (sets up the sequence based systems), Requires playerController input

Show GUI Shows the UI

Hide GUI Hides the UI

GUI Visible Simple Visibility test for the UI

Switch Platform (Switches which mode the UI will be in and how it will be displayed)
Note: this doesn't rotate the application into any one view it just make the UI responsive to that type of view.
Modes:
Desktop (default)
Mobile Portrait (Custom built for portrait mode reducing the X width where ever possible)
Mobile Landscape

=========================================================================================================================
Using the Sequence Unreal API

The Sequence Unreal plugin is broken up into a few different 
parts in order to manage all of the differing functionality
Here we'll go through each parts purpose then summarize how to use them all together below

**************************
NOTE: we automatically store credentials on disk in the
following cases:
1) On Successful Auth
2) On Successful Registering of a session
3) On Successful Closing of a session
**************************

************** USequenceWallet **************

To get a USequenceWallet call either:

USequenceWallet::Make(FCredentials_BE CredentialsIn) or
USequenceWallet::Make(FCredentials_BE CredentialsIn, FString ProviderURL)

Where the Credentials you give are the credentials you received from the UAuthenticator when [AuthSuccess] Fires,
OR you can use the call Auth->[GetStoredCredentials](), where Auth is of the type [UAuthenticator] if you are using
StoredCredentials please ensure they are valid by checking the wrapping Structs FStoredCredentials_BE.GetValid() flag
returned from [GetStoredCredentials], 
The providerURL is the url of the provider you wish to use.

Once you have your [USequenceWallet] UObject please ensure that you've registered the session using [RegisterSession]
before attempting to make other calls to the API.

SequenceAPI calls:

[Note]

We make use of TFunctions with some callbacks here I'll list some example syntax for them

TSuccessCallback
const TFunction<void(FString)> OnResponse = [Capturable variables](const FString& Response)
{
//callback body where we can process Response
};

FFailureCallback
const TFunction<void(FSequenceError)> OnFailureTest = [Capturable variables](const FSequenceError& Error)
{
//callback body where we can process Error
};

One thing to be aware of is keep an eye on capturables if you have lots of nested TFunctions it's very easy to miss
something and start over writing memory. If you require lots of nesting swapping to a more unreal esque approach using
UFUNCTION callbacks helps to avoid these problems similar to how things are done in [UAuthenticator.h/cpp]

[End Note]

/*
Used to sign the given string with sequence
TSuccessCallback<FSignedMessage> OnSuccess the TFunction that is used as a callback on successful signing the provided
struct contains the desired response

FFailureCallback Callback that is sent when something goes wrong during the signing process contains an error message
*/
SignMessage(const FString& Message, const TSuccessCallback<FSignedMessage>& OnSuccess, const FFailureCallback& OnFailure);

/*
Used to send a transaction through Sequence
TArray<TUnion<...>> Transactions , a list of transactions that are either Raw, ERC20, ERC721, ERC1155,

TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess called when the transaction is successful returns the Raw json object
due to the variety of responses we can receive from this call.

FFailureCallback Callback that is sent when something goes wrong during the SendTransaction process contains an error message
*/
SendTransaction(TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> Transactions, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);

/*
Used to Register a session after login
*NOTE* : this call must be done before any others in order to get valid registered credentials

OnSuccess<FString> Calls back on successful Registering with sequence
OnFailure Calls back on unsuccessful registering with sequence
*/
RegisterSession(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);

/*
Used to list the sessions on sequence

OnSuccess Calls back on success with a list of sessions
OnFailure calls when something went wrong
*/
ListSessions(const TSuccessCallback<TArray<FSession>>& OnSuccess, const FFailureCallback& OnFailure);

/*
Used to close the current session with sequence

OnSuccess calls back on successful close
OnFailure calls when something went wrong
*/
CloseSession(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);

//Indexer Calls//

/*
Used to get a ping back from the sequence app
@param ChainId, the chainId you wish to use

OnSuccess returns a bool indicating ping state
OnFailure error in pinging indexer
*/
void Ping(int64 chainID, TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure);

/*
Used to get version data back from the sequence app
@param ChainId, the chainId you wish to use

OnSuccess returns version data
OnFailure error getting version data from indexer
*/
void Version(int64 chainID, TSuccessCallback<FVersion> OnSuccess, FFailureCallback OnFailure);

/*
Used to get the runtime status of the sequence app
@param ChainId, the chainId you wish to use

OnSuccess returns runtime data
OnFailure error getting runtime status
*/
void RunTimeStatus(int64 chainID, TSuccessCallback<FRuntimeStatus> OnSuccess, FFailureCallback OnFailure);

/*
Used to get the chainID from the sequence app
@param ChainId, the chainId you wish to use

OnSuccess returns the chainId
OnFailure error getting chainId
*/
void GetChainID(int64 chainID, TSuccessCallback<int64> OnSuccess, FFailureCallback OnFailure);

/*
Used to get the Ether balance from the sequence app
@param ChainId, the chainId you wish to use
@param AccountAddr the accountAddress we want to get the balance for
@return the Balance

OnSuccess returns the Ether Balance
OnFailure error getting ether balance
*/
void GetEtherBalance(int64 chainID, FString accountAddr, TSuccessCallback<FEtherBalance> OnSuccess, FFailureCallback OnFailure);

/*
Gets the token balances from the sequence app
@param ChainId, the chainId you wish to use
@param FGetTokenBalancesArgs eg)
FGetTokenBalancesArgs args;
args.accountAddress = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
args.includeMetaData = true;

OnSuccess TokenBalances
OnFailure error getting tokenBalances
*/
void GetTokenBalances(int64 chainID, FGetTokenBalancesArgs args, TSuccessCallback<FGetTokenBalancesReturn> OnSuccess, FFailureCallback OnFailure);

/*
gets the token supplies from the sequence app

@param ChainId you wish to use
@param FGetTokenSuppliesArgs eg)
FGetTokenSuppliesArgs args;
args.contractAddress = testingContractAddress;
args.includeMetaData = true;

OnSuccess TokenSupplies
OnFailure error getting token supplies
*/
void GetTokenSupplies(int64 chainID, FGetTokenSuppliesArgs args, TSuccessCallback<FGetTokenSuppliesReturn> OnSuccess, FFailureCallback OnFailure);

/*
gets the token supplies map from the sequence app

@param ChainId, the chainId you wish to use
@param FGetTokenSuppliesMapArgs, eg)
FGetTokenSuppliesMapArgs args;
TMap<FString, FTokenList> tokenMap;
TPair<FString,FTokenList> item;
tokenMap.Add(item);
args.includeMetaData = true;
args.tokenMap = tokenMap;

OnSuccess TokenSuppliesMap
OnFailure Error getting token supplies map
*/
void GetTokenSuppliesMap(int64 chainID, FGetTokenSuppliesMapArgs args, TSuccessCallback<FGetTokenSuppliesMapReturn> OnSuccess, FFailureCallback OnFailure);

/*
Get the balance updates from the sequence app

@param ChainId, the chainId you wish to use
@param FGetBalanceUpdatesArgs the balance update arg struct sent to the indexer eg)
FGetBalanceUpdatesArgs args;
args.contractAddress = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
args.page.page = 10;
args.page.more = true;

OnSuccess BalanceUpdates
OnFailure error getting balance updates
*/
void GetBalanceUpdates(int64 chainID, FGetBalanceUpdatesArgs args, TSuccessCallback<FGetBalanceUpdatesReturn> OnSuccess, FFailureCallback OnFailure);

/*
get transaction history from the sequence app

@Param ChainId the chainId you want to use
@Param FGetTransactionHistoryArgs, the transaction history argument struct required to send to the indexer
eg)
FGetTransactionHistoryArgs args;
args.filter.accountAddress = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
args.includeMetaData = true;
args.page.page = 0;
args.page.more = true;

OnSuccess TransactionHistory
OnFailure error getting transaction History
*/
void GetTransactionHistory(int64 chainID, FGetTransactionHistoryArgs args, TSuccessCallback<FGetTransactionHistoryReturn> OnSuccess, FFailureCallback OnFailure);

//Provider Calls (Note to use this properly you need to specify a provider URL when making the USequenceWallet)

/*
Returns information about a block by block number.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_getblockbynumber
@param uint64 Number, the block we are querying for
@return the JsonResponse of the Block
*/
void BlockByNumber(uint64 Number, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);

/*
Returns information about a block by block number.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_getblockbynumber
@param EBlockTag Tag, the block we are querying for
@return the JsonResponse of the Block
*/
void BlockByNumber(EBlockTag Tag, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);

/*
Returns information about a block by block number.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_getblockbynumber
@return the block number
*/
void BlockNumber(TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure);

/*
Returns information about a block by hash.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_getblockbyhash
@param FHash256 Hash, DATA, 32 Bytes - Hash of a block.
@return JsonResponse of the Block, Boolean - If true it returns the full transaction objects, if false only the hashes of the transactions.
*/
void BlockByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);

/*
Returns the information about a transaction requested by transaction hash.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_gettransactionbyhash
@param FHash256 Hash, DATA, 32 Bytes - hash of a transaction.
@return JsonResponse, Object - A transaction object, or null when no transaction was found
*/
void TransactionByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);

/*
Returns the number of transactions sent from an address.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_gettransactioncount
@param FAddress Addr, DATA, 20 Bytes - address.
@param uint64 Number, QUANTITY|TAG - integer block number, or the string "latest", "earliest" or "pending", see the default block parameter
@return uint64, QUANTITY - integer of the number of transactions send from this address.
*/
void TransactionCount(FAddress Addr, uint64 Number, TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure);

/*
Returns the number of transactions sent from an address.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_gettransactioncount
@param FAddress Addr, DATA, 20 Bytes - address.
@param EBlockTag Tag, QUANTITY|TAG - integer block number, or the string "latest", "earliest" or "pending", see the default block parameter
@return uint64, QUANTITY - integer of the number of transactions send from this address.
*/
void TransactionCount(FAddress Addr, EBlockTag Tag, TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure);

/*
Returns the receipt of a transaction by transaction hash.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_gettransactionreceipt
@param FHash256 Hash, DATA, 32 Bytes - hash of a transaction
@return FTransactionReceipt, Object - A transaction receipt object, or null when no receipt was found
*/
void TransactionReceipt(FHash256 Hash, TSuccessCallback<FTransactionReceipt> OnSuccess, FFailureCallback OnFailure);

/*
Creates new message call transaction or a contract creation for signed transactions.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_sendrawtransaction
@param FString Data, DATA, The signed transaction data.
@return FUnsizedData, DATA, The signed transaction data.
*/
void SendRawTransaction(FString Data, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);

/*
Executes a new message call immediately without creating a transaction on the block chain. Often used for executing read-only smart contract functions, for example the balanceOf for an ERC-20 contract.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_call
@param FContractCall ContractCall, Object - The transaction call object
@param uint64 Number,
@return FUnsizedData, DATA - the return value of executed contract.
*/
void Call(FContractCall ContractCall, uint64 Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);

/*
Executes a new message call immediately without creating a transaction on the block chain. Often used for executing read-only smart contract functions, for example the balanceOf for an ERC-20 contract.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_call
@param FContractCall ContractCall, Object - The transaction call object
@param EBlockTag Number,
@return FUnsizedData, DATA - the return value of executed contract.
*/
void Call(FContractCall ContractCall, EBlockTag Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);

/*
Gets the ChainId of the set provider
@return the ChainId of the provider
*/
void ChainId(TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure);

/*
Returns an estimate of the current price per gas in wei. 
For example, the Besu client examines the last 100 blocks and returns the median gas unit price by default.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_gasprice
@return FUnsizedData, QUANTITY - integer of the current gas price in wei.
*/
void GetGasPrice(TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);

/*
Returns an estimate of the current price per gas in wei.
For example, the Besu client examines the last 100 blocks and returns the median gas unit price by default.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_gasprice
@param FContractCall ContractCall, (used to estimate gas for the given contract call)
@return FUnsizedData, QUANTITY - integer of the current gas price in wei.
*/
void EstimateContractCallGas(FContractCall ContractCall, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);

/*
Returns an estimate of the current price per gas in wei.
For example, the Besu client examines the last 100 blocks and returns the median gas unit price by default.
https://ethereum.org/en/developers/docs/apis/json-rpc/#eth_gasprice
@param FAddress From, (used to estimate gas for the given address & ByteCode)
@param FString ByteCode, (used to estimate gas for the given address & ByteCode)
@return FUnsizedData, QUANTITY - integer of the current gas price in wei.
*/
void EstimateDeploymentGas(FAddress From, FString Bytecode, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);

/*
Used to deploy contracts
@param FString ByteCode,
@param FPrivateKey PrivKey
@param ChainId
@return FAddress of the deployed contract
*/
void DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallback<FAddress> OnSuccess, FFailureCallback OnFailure);

/*
Used to deploy contracts
@param FString ByteCode,
@param FPrivateKey PrivKey
@param ChainId
@return FAddress of the deployed contract & the hash of the contract after deployment
*/
void DeployContractWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallbackTuple<FAddress, FUnsizedData> OnSuccess, FFailureCallback OnFailure);

/*
Used to extract a nonce value from a given block number
@param uint64 Number, The block number to use
@return FBlockNonce, the nonce value for that block
*/
void NonceAt(uint64 Number, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure);

/*
Used to extract a nonce value from a given tag
@param EBlockTag Tag, the tag to use
@return FBlockNonce the nonce value for that block
*/
void NonceAt(EBlockTag Tag, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure);

/*
Returns the header of the given block id
@param uint64 Id, the id of the block we want to get the header for
@return FHeader the header of the block
*/
void HeaderByNumber(uint64 Id, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure);

/*
Returns the header of the given block Tag
@param EBlockTag Tag, the Tag of the block we want to get the header for
@return FHeader the header of the block
*/
void HeaderByNumber(EBlockTag Tag, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure);

/*
Returns the header of the given block Hash
@param FHash256 Hash, the Hash of the block we want to get the header for
@return FHeader the header of the block
*/
void HeaderByHash(FHash256 Hash, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure);

/*
Contract call for a nonView function
@param FEthTransaction transaction,
@param FPrivateKey PrivateKey,
@param int32 ChainID,
@return FUnsizedData from the nonview call
*/
void NonViewCall(FEthTransaction transaction, FPrivateKey PrivateKey, int ChainID, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);

************** USequenceAPI **************

************** Working with RawBytes in Unreal **************

There are alot of cases where you'll be interacting with raw ByteData, we commonly wrapped these datums up in

************** FUnsizedData **************

/*
FUnsizedData is used to hold byte data of an arbitary length,
Inherits from FBinaryData
*/

/*
Converts a hex string (no leading 0x) to a Number of
arbitrary size
@param FString Hex, The hex string you'd like to convert
@return the FUnsizedData struct
*/
FUnsizedData HexStringToBinary(FString Hex); // Hex String to Number of arbitrary size

/*
Converts a string straight into FUnsizedData (raw bytes)
@param FString string, the string you want to convert
@return FUnsizedData, the FUnsizedData struct
*/
FUnsizedData StringToUTF8(FString String);

/*
Converts FUnsizedData into an FString
@param FUnsizedData BinaryData the data you want to convert
@return FString, the resulting FString
*/
FString UTF8ToString(FUnsizedData BinaryData);

/*
Converts the FUnsizedData object into a hex FString
@param none,
@return FString, the resulting hex FString.

Called like FUnsizedData test;
test.ToHex();
*/
FString ToHex();

/*
Accessing underlying uint8 * in FUnsizedData
@return uint8 *, The underlying byte pointer
*/
uint8 * Ptr();

/*
Accessing the underlying TArray in FUnsizedData
@return TSharedPtr<TArray<uint8>>, The underlying TSharedPtr<TArray<uint8>>
*/
TSharedPtr<TArray<uint8>> Arr();

************** FUnsizedData **************

************** TSizedData **************

/*
Templated Datatype for any data type of a fixed size for example an ethereum address is 20 bytes
which is defined in FAddress which is a child of TSizedData
*/

template<ByteLength TSize>
struct TSizedData : FBinaryData
{
const static ByteLength Size = TSize;
virtual ByteLength GetLength() const override;
FUnsizedData Copy() const; // This creates new data that must be freed
explicit operator FUnsizedData() const { return Copy(); }
static TSizedData Empty();
}; // Data with set sizes

template <ByteLength Size>
inline ByteLength TSizedData<Size>::GetLength() const
{
return Size;
}

template <ByteLength Size>
FUnsizedData TSizedData<Size>::Copy() const
{
FUnsizedData data = FUnsizedData::Empty();
data.Arr.Get()->Append(this->Arr);
return data;
}

template <ByteLength TSize>
TSizedData<TSize> TSizedData<TSize>::Empty()
{
return TSizedData{};
}

************** TSizedData **************

************** FHash256 **************

/*
Used to represent a 32 byte hash (256 bits)
*/

/*
Used to create a new FHash256
@return FHash256
*/
static FHash256::New();

/*
Provide a static 32 byte TArray to initialize an
FAddress
@param TStaticArray<uint8,32> Arr, the initializing array
@return the FHash256 initialized with the given byte data
*/
static FHash256::From(TStaticArray<uint8, 32> &Arr);

/*
Provide a 32 byte TArray to initialize the FHash256
with
@param TArray<uint8> Arr, the initializing TArray of 32 bytes in length
@return the FHash256 initialized with the given data
*/
static FHash256 From(TArray<uint8> &Arr);

/*
Provide a hex FString that works out to 32 bytes of data to
initialize the FHash256 with
@param FString Str, the 32 hex byte string
@return the FHash256 initialized from the given hex string
*/
static FHash256 From(FString Str);


************** FHash256 **************

************** FAddress **************

/*
Used to store a 20 bytes Ethereum Address
*/

/*
Creates a new FAddress
@return a new FAddress
*/
FAddress::New();

/*
Provide a static 20 byte TArray to initialize an
FAddress
@param TStaticArray<uint8, 20> Arr, the initializing static TArray
@return the FAddress initialized from the given TStaticArray
*/
static FAddress::From(TStaticArray<uint8, 20> &Arr);

/*
Provide a hex FString that works out to 20 bytes of data to
initialize the FAddress with
@param FString Str, the 20 hex byte FString
@return the FAddress initialized from the given Hex byte FString
*/
static FAddress From(FString Str);

************** FAddress **************

************** FPublicKey **************

/*
Used to store a 64 byte Ethereum public Key
*/

/*
Creates a new FPublicKey
@return a new FPublicKey
*/
FPublicKey::New();

/*
Provide a static 64 byte TArray to initialize an
FPublicKey
@param TStaticArray<uint8, 64> Arr, the 64 Array used to initialize the FPublicKey
@return the FPublicKey initialized with the given TStaticArray
*/
static FPublicKey::From(TStaticArray<uint8, 64> &Arr);

/*
Provide a hex FString that works out to 64 bytes of data to
initialize the FPublicKey with
@param FString str, the 64 hex byte FString,
@return the FPublicKey Initialized from the given FString
*/
static FPublicKey From(FString Str);

************** FPublicKey **************

************** FPrivateKey **************

/*
Used to store a 32 byte Ethereum private key
*/

/*
Creates a new FPrivateKey
@return a new FPrivateKey
*/
FPrivateKey::New();

/*
Provide a static 32 byte TArray to initialize an
FPrivateKey
@param TStaticArray<uint8, 32> Arr, the 32 byte Array used to initialize the FPrivateKey
@return the FPrivateKey initialized with the given TStaticArray
*/
static FPrivateKey::From(TStaticArray<uint8, 32> &Arr);

/*
Provide a hex FString that works out to 32 bytes of data to
initialize the FPrivateKey with
@param FString Str, the 32 hex byte FString
@return the FPrivateKey initialized with the FString
*/
static FPrivateKey From(FString Str);

************** FPrivateKey **************

************** FBloom **************

struct FBloom final : TSizedData<256>
{
static FBloom New(); // This creates new data that must be freed
static FBloom From(TStaticArray<uint8, 256> &Arr);
static FBloom From(FString Str);
};

************** FBloom **************

************** FBlockNonce **************

struct FBlockNonce final : TSizedData<8>
{
static FBlockNonce New(); // This creates new data that must be freed
static FBlockNonce From(TStaticArray<uint8, 8> &Arr);
static FBlockNonce From(FString Str);
void Increment() const;
};

************** FBlockNonce **************

************** Unreal Conversion Calls **************

/*
Converts the given byte data into a hex string
@param uint8 * Data, the byte data pointer
@param int32 Length, the number of byte the byte data pointer points to
@return FString, the hex FString
*/
BytesToHex(uint8 * Data, int32 length);

/*
Takes a given set of Bytes and converts it to a Hex FString
@param FString string, the hex string,
@param uint8 * Bytes, the byte array where we will be storing our data
@return in32, the number of bytes copied
*/
HexToBytes(FString& string, uint8 * Bytes);

************** Unreal Conversion Calls **************

************** Working with RawBytes in Unreal **************

************** ABI **************



	// CALL DATA
	FString FunctionSignature = "balanceOf(address,uint256)";
	TFixedABIData Account = ABI::Address(FAddress::From("0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2"));
	TFixedABIData Id = ABI::UInt32(0x01);
	TArray<ABIEncodeable*> Arr;
	Arr.Add(&Account);
	Arr.Add(&Id);
	const FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);	

************** ABI **************

************** UAuthenticator **************

UAuthenticator

This UObject's purpose is to handle authenticating with Sequence Systems

Key parts
Delegates:

/*
This event is fired when the authenticator requires a
code from the user in the UI, if you're using a custom UI
collect it and send it to the UAuthenticator Object using EmailLoginCode(FString CodeIn)
*/
AuthRequiresCode

/*
This event fires out when an error occurs in the auth
process, this can be for many reasons, timeout bad settings
etc, in general you can bind to this to let you know when you
should show an error prompt if using a custom UI
*/
AuthFailure

/*
This event fires when auth is successful the event will
also include the authenticated FCredentials_BE struct
*/
AuthSuccess

/*
Gets the Appropriate signin URL for SSO depending on the
provider IE) Google or Apple
*/
GetSigninURL(ESocialSigninType Type)

/*
This is for dealing with Social based Login,
(WIP)
*/
SocialLogin(FString IDTokenIn)

/*
Used to handle email based login
*/
EmailLogin(FString EmailIn)

/*
Used to respond to the event AuthRequiresCode
include the needed Code in this function to
continue email based auth
*/
EmailLoginCode(FString CodeIn)

/*
Used to get the credentials currently stored on disk
the FStruct returned is a wrapper for FCredentials_BE,
the bool value IsValid is also included so you can check
if the credentials on disk were valid or not
*/
FStoredCredentials_BE GetStoredCredentials()

/*
Used to store the given credentials on disk
*/
StoreCredentials(FCredentials_BE CredentialsIn)

************** UAuthenticator **************

************** Waas Configuration **************

To configure Waas with your particular information please refer to the following file [Config.h] under
SequencePlugin/Source/SequencePlugin/Private/Config/Config.h

In this file you'll find [FSequenceConfig] which contains 3 values you'll need to update

VITE_SEQUENCE_WAAS_CONFIG_KEY which you get from SequenceBuilder (need web link)

ProjectAccessKey which you get from SequenceBuilder (need web link)

WaasVersion which gets set to whichever version of Waas you are using (default is 1.0.0)

************** Waas Configuration **************

************** Packaging **************

To set your system up for Packaging please refer to the following links
For Windows, Mac
https://docs.unrealengine.com/5.0/en-US/packaging-unreal-engine-projects/#:~:text=Clicking%20File%20%3E%20Package%20Project%20%3E%20Packaging,options%20for%20the%20packaging%20feature.&text=The%20build%20configuration%20to%20compile,a%20code%20project%2C%20select%20DebugGame.
For Android
https://docs.unrealengine.com/4.27/en-US/SharingAndReleasing/Mobile/Android/PackagingAndroidProject/
For IOS
https://docs.unrealengine.com/5.0/en-US/packaging-ios-projects-in-unreal-engine/

For IOS you also need to setup provisioning refer to these docs to achieve that:

https://docs.unrealengine.com/5.1/en-US/setting-up-ios-tvos-and-ipados-provisioning-profiles-and-signing-certificates-for-unreal-engine-projects/

Unreal <-> Xcode Specifics:

************** Packaging **************