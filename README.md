Sequence Unreal SDK
===================

This plugin requires the modern xcode setting to be enabled to build on MacOS.
See [the following link](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-modern-xcode-in-unreal-engine-5.3-and-newer) for more details.

===================

This SDK allows easy integration of Sequence Embedded Wallet from any Unreal Framework project.

## Manually Upgrading from previous versions
IF you are using release Beta_1_0_3 or older please backup the values you stored in `PluginConfig/Config.h` or `Config/Config.h`
After you've backed up your configuration data, Delete the entirety of the SequencePlugin directory. And drop
in the new updated version.

We now are opting to use .ini files to store configurations for the plugin rather than storing them in the plugin itself.
This will make integrating updates to the plugin much simpler.

To do this please go to [YourProjectDirectory]/Config And create a file named **[SequenceConfig.ini]**

Within **[SequenceConfig.ini]** add the following lines:

      [/Script/Sequence.Config]
      FallbackEncryptionKey = ""
      WaaSTenantKey = ""
      ProjectAccessKey = ""
      GoogleClientID = ""
      AppleClientID = ""
      FacebookClientID = ""
      DiscordClientID = ""
      RedirectUrl = "https://api.sequence.app"

Here is where you'll fill in the various configuration values for the plugin.
For the time being we don't support Facebook or Discord authentication so feel free to ignore those 2 clientId's for now.

### Upgrading to Unreal 5.4
If your project is currently on 5.2 or 5.3, then for this version you'll want to upgrade to 5.4.
To do this close the Unreal Editor, Close your Code Editor. Right click on your Unreal Project
file. Then select Switch Unreal Engine Version, Choose 5.4 and click okay. Once done. Open your
code editor and rebuild your source code.

As way of future proofing we recommend installing visual studio 2022 in accordance with [these docs](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine)
as Unreal 5.4 and onward will be dropping support for visual studio 2019.

### YourProject.Build.cs

In some instances your build.cs may not update properly for your project when this happens you'll be unable to import plugin,
if this occurs in your Projects Build.cs file please check the private Dependency module such that it includes "SequencePlugin":

    public SequenceTest(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        //The line below is what you want
		PrivateDependencyModuleNames.AddRange(new string[] { "SequencePlugin" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}

## Credentials

Before you can use this plugin, you need to acquire the following credentials from [Sequence](https://sequence.xyz/builder)

- `WaaSTenantKey`
- `ProjectAccessKey`

You can then add these credentials in the **[SequenceConfig.ini]** file under [YourProject]/Config/SequenceConfig.ini

## Security

You must provide a 32 character encryption key in the **[SequenceConfig.ini]** file under [YourProject]/Config/SequenceConfig.ini
under the config variable `FallbackEncryptionKey`

In order to prevent tampering with data you must encrypt your packaged project using Unreals packaging settings
You can refer to [these docs](https://dev.epicgames.com/documentation/en-us/unreal-engine/packaging-unreal-engine-projects?application_version=5.3)

Note: For generating secure keys feel free to use tools such as:
Lastpass Password generator or equivalent,
Encryption Key Generators,
etc

***

## Getting Started With the Builtin GUI

1) Once you have the `SequencePlugin` folder, you'll need to go to your project directory and create a `Plugins` folder in it,
   then copy over the `SequencePlugin` folder into the `Plugins` folder. If a `Plugins` folder already exists just copy the `SequencePlugin` folder into it.

2) Launch your project, then allow it to update the UProject Settings.

3) To find the `SequencePlugin` content folder in your content drawer enable view plugin content

4) If you wish to use the in built sequence UI for login you have to do the following:

   a) Create a C++ Class that Inherits from **[Pawn]** If you don't know how to do this refer to the doc [Creating C++ Classes in Unreal](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-the-cplusplus-class-wizard-in-unreal-engine?application_version=5.2),
   for the purpose of these docs I'll refer to the C++ Class created here as the **[C++ Parent]**

   b) In **[C++ Parent]** .h file include the Header **[SequenceAPI.h]** this will allow you to access the **[USequenceWallet]**

   c) Create a BlueprintCallable function within the **[C++ Parent]** that accepts **[FCredentials_BE]** as a Parameter.

   d) Create a Blueprint that inherits from **[C++ Parent]**, Then Attach the following Actor component to it **[AC_SequencePawn_Component]**. For in depth specifics on how to setup this blueprint
   please refer to the demonstration BP Graph [Image](ReadmeImages/Example_BP.PNG), this is the BP Graph of **[BP_CustomSpectatorPawn]** contained within the plugins content folder, & serves as a template for your
   own Blueprint graph.

   The important part here is forwarding the Credentials received from the inbuilt UI to your **[C++ Parent]** by binding to the delegate from **[AC_SequencePawn_Component]**,
   that gives you Credentials **[Auth_Success_Forwarder]** & Calling your Blueprint Callable C++ function.
   You can do this by swapping the SetupCredentials BlueprintCallable Function for your own Blueprint Callable Function from **[C++ Parent]**.

   d i) For those who aren't familiar with Unreal's Blueprint system you can create a blueprint by right clicking in the content
   drawer, then click blueprint class. Within the blueprint class selector select the All Classes dropdown & search  
   for your **[C++ Parent]** class you just made.

   d ii) For those who aren't familiar with Unreal's delegate system, There will be a red empty box on a delegate you'll wish
   to bind to. Click on this box and drag out into the blueprint editor. From the menu that appears Click the Add Event dropdown
   then click add custom event.

Note: You can simply duplicate the **[BP_CustomSpectatorPawn]** but since it & its parent class reside within the realm of the plugin,
during updates all code you place there could potentially be lost. These are here as a reference for how things should be done. If you wish to use these components
it's recommended you duplicate the BP_CustomSpectatorPawn out of the plugin folder, then update it's parent class to a C++ class of your own making that also
resides outside the plugins content folder.

5) Some additional setup of the GameMode will need to be done prior to any UI showing up. The SequencePlugin comes bundled with an example
   GameMode **[GM_Sequence]** stored within **[Demonstration]** in the plugin content folder. Duplicate this GameMode and move it outside the plugin folder.
   Then open up **[GM_Sequence]** and set the DefaultPawn to the Pawn Blueprint you've just made.

6) Lastly in Project Settings you'll need to set this GameMode as the default GameMode. Specifically in ProjectSettings -> Maps & Modes

If you don't know what some of the Entities referred to above are / how they work in unreal please refer to the following Docs:
To learn more about GameModes and GameMode state refer to [these docs](https://dev.epicgames.com/documentation/en-us/unreal-engine/game-mode-and-game-state-in-unreal-engine?application_version=5.2)
To learn more about Pawns refer to [these docs](https://dev.epicgames.com/documentation/en-us/unreal-engine/pawn-in-unreal-engine?application_version=5.2)
[Pawns](https://docs.unrealengine.com/4.27/en-US/InteractiveExperiences/Framework/Pawn/)
[Components](https://dev.epicgames.com/documentation/en-us/unreal-engine/components-in-unreal-engine?application_version=5.2)
[PlayerController](https://dev.epicgames.com/documentation/en-us/unreal-engine/player-controllers-in-unreal-engine?application_version=5.2)
[UI in Unreal](https://dev.epicgames.com/documentation/en-us/unreal-engine/creating-widgets-in-unreal-engine?application_version=5.2)
[C++ & Blueprints](https://dev.epicgames.com/documentation/en-us/unreal-engine/cpp-and-blueprints-example?application_version=5.2)
[Creating C++ Classes in Unreal](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-the-cplusplus-class-wizard-in-unreal-engine?application_version=5.2)

##### Take away notes on setup
This isn't the only way you can setup the Builtin GUI, this is here as a quick start reference for those
just getting started with unreal.

### Customizing the builtin GUI

In the folder located at All/Plugins/SequencePlugin Content/Core/Style you'll find a struct F_SequenceUIStyle,
In the default values section of this struct you'll be able to update the colours and images displayed throughout the UI.
For beta we currently only read from Sequence_Style_Dark_Mode

### Custom UI Integration

In a C++ UObject with a series of pass through **[UFUNCTIONS]** setup similarly to **[SequenceBackendManager.h/.cpp]**. Each of these calls are implemented in **[UAuthenticator]** you just need to pass through the data with YOUR UAuthenticator UObject

```clike
/*
   Used to initiate mobile Social Signin
   (No other calls need to be made to complete mobile SSO)
*/
void InitiateMobileSSO(const ESocialSigninType& Type)

/*
   Optional Call,
   Used to set a custom encryptor implementation for the Authentication Process
*/
void SetCustomEncryptor(UGenericNativeEncryptor * EncryptorIn);

/*
   This call is for generating a login URL for Desktop based Social Signin
   the received URL is fed into a WebBrowser to begin the login process
*/
FString GetLoginURL(const ESocialSigninType& Type); 

/*
   This is call is for undergoing social login once an ID_Token has been collected.
*/
void SocialLogin(const FString& IDTokenIn);

/*
   This Call is made after you've collected the email address from the Users in the UI
   The Delegate **[AuthRequiresCode]** will fire when a code is ready to be received
   by the UAuthenticator
*/
void EmailLogin(const FString& EmailIn);

/*
   This is call is made after the Delegate **[AuthRequiresCode]** is fired
   The Code collected from the User in the GUI is sent in via this call
*/
void EmailCode(const FString& CodeIn);

/*
   Optional call used to retrieve stored credentials on disk
*/
FStoredCredentials_BE GetStoredCredentials() const;

/*
   Optional call used to check if the credentials on disk are valid or not
*/
bool StoredCredentialsValid();
```

To start you'll want to create a **[UAuthenticator]** UObject like so **[UAuthenticator * Auth = NewObject<UAuthenticator>()]**, this UObject manages the authentication side of Sequence.

Be sure to bind to the Delegates for **[AuthSuccess]**, **[AuthFailure]**, **[AuthRequiresCode]** prior to making any signin calls You can bind to these delegates like so:

```clike
this->authenticator->AuthRequiresCode.AddDynamic(this, &AYourClass::YourCallReadyToReceiveCode);
this->authenticator->AuthFailure.AddDynamic(this, &AYourClass::YourCallShowAuthFailureScreen);
```

In the case of **[AuthSuccess]** since a parameter is also passed we bind to it like this

```clike
FScriptDelegate del;
del.BindUFunction(this, "CallShowAuthSuccessScreen");
this->authenticator->AuthSuccess.Add(del);
```

Note: Replace the usage of the SequenceBackendManager.h/.cpp with you're own when building a custom GUI,
it is only used here as a reference in the event more context is needed with these instructions.
Where **[CallShowAuthSuccessScreen]** is defined in `SequenceBackendManager.h` as an example like so:

```clike
UFUNCTION()
void CallShowAuthSuccessScreen(const FCredentials_BE& CredentialsIn);
```

And in `SequenceBackendManager.cpp` like so:

```clike
void ASequenceBackendManager::CallShowAuthSuccessScreen(const FCredentials_BE& CredentialsIn) 
{
this->Credentials = CredentialsIn;
if (this->ShowAuthSuccessDelegate.IsBound())
  this->ShowAuthSuccessDelegate.Broadcast(Credentials);
else
  UE_LOG(LogTemp, Error, TEXT("**[Nothing bound to: ShowAuthSuccessDelegate]**"));
}
```

### Email based Authentication With CustomUI

1) To start email based authentication you'll start it with this call **[EmailLogin(const FString& EmailIn)]**, supplying an email you've collected from the User in your GUI.

2) Next **[AuthRequiresCode]** will fire when the **[UAuthenticator]** is ready to receive the Code from your UI. Collect this code from your GUI and send it to the authenticator using **[EmailCode(CodeIn)]**.

3) Finally **[AuthSuccess]** will fire with a Credentials_BE struct as a parameter. You are done Email Based Auth.

### Social Signin based Authentication on Desktop With CustomUI

1) To start SSO based authentication with desktop you can either use your own implementation to get the necessary id_token or you can make use of Unreal's web browser plugin.

2) To get the URL to navigate to you can use the UAuthenticator supplied call **[FString GetSigninURL(const ESocialSigninType& Type)]** where Type is the social login type you wish to use

3) With whatever implementation you chose you can forward the collected id_token to the UAuthenticator object with **[SocialLogin(const FString& IDTokenIn)]**, after which **[AuthSuccess]** will fire and you're done desktop based SSO.

### Social Signin based Authentication on Mobile With CustomUI

1) To start mobile SSO you will need to make use of the **[UAuthenticator::InitiateMobileSSO(const ESocialSigninType& Type)]**
   where type is the Type of SSO you want to use. IE) Google or Apple, for the time being Discord & Facebook aren't supported.
   This function call is all that's required for Mobile SSO.

### Android SSO Requirements

Google:

In order to be able to properly use Google Auth, create and place the Keystore file by following [these instructions](https://docs.unrealengine.com/5.1/en-US/signing-android-projects-for-release-on-the-google-play-store-with-unreal-engine/).

You will also need to generate an **[Android client ID]** and a **[Web Application client ID]** for your application. And place the **[Web Application client ID]** in the [YourProject/Config/SequenceConfig.ini], [GoogleClientID] field.

Refer to [these docs](https://developers.google.com/identity/one-tap/android/get-started#api-console) to generate **[Android client ID]** and **[Web Application client ID]**.

[This guide](https://developers.google.com/android/guides/client-auth) helps explain how to collect SHA-1 key fingerprints for the **[Android client ID]**.

Apple: Please ensure you have a proper **[AppleClientID]** set in **[YourProject/Config/SequenceConfig.ini]**

### IOS SSO Requirements
Google: Please ensure you have a proper **[GoogleClientID]** set in **[YourProject/Config/SequenceConfig.ini]**

Apple: Please ensure you have a proper **[AppleClientID]** set in **[YourProject/Config/SequenceConfig.ini]**,
be sure you register and set your bundle identifier properly for your app

### Apple Specific SSO Requirements
For Apple SSO to work please be sure to register the **[RedirectUrl]** in **[YourProject/Config/SequenceConfig.ini]** appropriately for your app.

### Sequence API

In order to gain access to the SequenceAPI be sure to #include "Sequence/SequenceAPI.h"
After you've completed initial authentication and have intercepted the credentials either through your UI or ours, to use the Sequence API you'll need to create a **[USequenceWallet*]** by using:

```clike
/*
   Automatically tries to read stored credentials on disk and initialize with them
   if none are found returns a TOptional<USequenceWallet*> Pointer without any set Credentials
*/
USequenceWallet::Get()

const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
if (WalletOptional.IsSet() && WalletOptional.GetValue())
{
   USequenceWallet * Wallet = WalletOptional.GetValue();
   //Use here
}

or

/*
   returns a TOptional<USequenceWallet*> Pointer set with the 
   given Credentials
*/
USequenceWallet::Get(const FCredentials_BE& Credentials)

const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
if (WalletOptional.IsSet() && WalletOptional.GetValue())
{
   USequenceWallet * Wallet = WalletOptional.GetValue();
   //Use here
}

or

/*
   returns a TOptional<USequenceWallet*> Pointer set with the 
   given Credentials & ProviderUrl
*/
USequenceWallet::Get(const FCredentials_BE& Credentials,const FString& ProviderUrl);

const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials,"ProviderUrl");
if (WalletOptional.IsSet() && WalletOptional.GetValue())
{
   USequenceWallet * Wallet = WalletOptional.GetValue();
   //Use here
}
```

Once you have your **[USequenceWallet]** you can feel free to call any of the functions Supplied by the object, as the register call is now automatically done for you.

### USequenceWallet, Static Access & Persistence
USequenceWallet is now a Subsystem of GameInstance, what this means is not only can you access it from anywhere
as if it was a static variable in your C++ code. But it also persists for the lifetime of your game. That is
no data is reset when a level is changed in your games!

### USequenceWallet Functions

### Example SignMessage
##### Used to Sign a message

	const TSuccessCallback<FSignedMessage> OnResponse = [=] (const FSignedMessage& Response)
	{
		//Response is the signed message
    };

	const FFailureCallback OnFailure = [=](const FSequenceError& Error)
	{
		UE_LOG(LogTemp,Display,TEXT("Error Message: %s"),*Error.Message);
    };
    
    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
       const FString Message = "Hi";
       Api->SignMessage(Message,OnResponse,OnFailure);
    }

### Example SendTransaction
##### Used to send a transaction / perform contract calls
Note: if you want call contracts with the Raw type you'll want include the header
`#include "ABI/ABI.h"` in order to use the ABI to encode the data for a contract call.

	const FFailureCallback OnFailure = [=](const FSequenceError& Error)
	{
		UE_LOG(LogTemp,Display,TEXT("Error Message: %s"),*Error.Message);
	};
	
    //Create the Transaction object list
	TArray<TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>> Txn;
	
    //Create the transactions you wish to perform

    //ERC20
	FERC20Transaction T20;
	T20.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T20.tokenAddress = "0x2791Bca1f2de4661ED88A30C99A7a9449Aa84174";
	T20.value = "1000";

    //ERC721
	FERC721Transaction T721;
	T721.safe = true;
	T721.id = "54530968763798660137294927684252503703134533114052628080002308208148824588621";
	T721.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T721.tokenAddress = "0xa9a6A3626993D487d2Dbda3173cf58cA1a9D9e9f";

    //ERC1155
	FERC1155Transaction T1155;
	T1155.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T1155.tokenAddress = "0x631998e91476DA5B870D741192fc5Cbc55F5a52E";
	
	FERC1155TxnValue Val;
	Val.amount = "1";
	Val.id = "66635";
	T1155.vals.Add(Val);

    //Raw (Example contract call)
    FString FunctionSignature = "balanceOf(address,uint256)";
	TFixedABIData Account = ABI::Address(FAddress::From("0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2"));
	TFixedABIData Id = ABI::UInt32(0x01);
	TArray<ABIEncodeable*> Arr;
	Arr.Add(&Account);
	Arr.Add(&Id);
	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = "0x64d9f9d527abe2a1c1ce3fada98601c4ac5bfdd2";
	T.value = "0";
	
    //Now append your transaction requests to the Txn object
    
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T20));//ERC20
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T721));//ERC721
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T1155));//ERC1155
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T));//ContractCall

    //Now send the transaction
    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
      USequenceWallet * Api = WalletOptional.GetValue();
       Api->SendTransaction(Txn,[=](const FTransactionResponse& Transaction)
       {
          TSharedPtr<FJsonObject> Json = Transaction.Json;
       	  TSharedPtr<FJsonObject> Receipt = Transaction.Receipt;
       	  TSharedPtr<FJsonObject> NativeReceipt = Transaction.NativeReceipt;
       	  TSharedPtr<FJsonObject> Request = Transaction.Request;
       	  TArray<TSharedPtr<FJsonValue>> Simulations = Transaction.Simulations;
       	  FString TxHash = Transaction.TxHash;
       	  FString IdentifyingCode = Transaction.IdentifyingCode;
       	  FString MetaTxHash = Transaction.MetaTxHash;
	   },OnFailure);
    }

### Example ListSessions
##### Lists the active sessions

    const TSuccessCallback<TArray<FSession>> OnSuccess = [=](TArray<FSession> Response)
    {
       //Response is a list of Sessions
    };

	const FFailureCallback OnFailure = [=](const FSequenceError& Error)
	{
		UE_LOG(LogTemp,Display,TEXT("Error Message: %s"),*Error.Message);
    };

    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them  
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
	   USequenceWallet * Api = WalletOptional.GetValue();
       Api->ListSessions(OnSuccess,OnFailure);
    }

### Example CloseSession
##### Closes the session

	const TFunction<void(FString)> OnSuccess = [=](const FString& Response)
	{
		//Response is just a confirmation string
    };

	const FFailureCallback OnFailure = [=](const FSequenceError& Error)
	{
		UE_LOG(LogTemp,Display,TEXT("Error Message: %s"),*Error.Message);
    };

    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
	   USequenceWallet * Api = WalletOptional.GetValue();
	   Api->CloseSession(OnSuccess,OnFailure);
    }

### Example SignOut
##### Closes the session & clears out cached credentials with blank ones

    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
	   USequenceWallet * Api = WalletOptional.GetValue();
       Api->SignOut();
    }

### Example RegisterSession
##### Used to register a session (done automatically for you by UAuthenticator)

    const TFunction<void(FCredentials_BE)> OnSuccess = [=](FCredentials_BE Response)
    {
        //Successful registration
    };

	const FFailureCallback OnFailure = [=](const FSequenceError& Error)
	{
		UE_LOG(LogTemp,Display,TEXT("Error Message: %s"),*Error.Message);
    };

    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
	   USequenceWallet * Api = WalletOptional.GetValue();
	   Api->RegisterSession(OnSuccess,OnFailure);
    }

### Example GetWalletAddress
#### Gets the wallet address currently being used

    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
	   USequenceWallet * Api = WalletOptional.GetValue();
       Api->GetWalletAddress();
    }

### Example GetNetworkId
#### Gets the network id being used

    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
       Api->GetNetworkId();
    }

### Example UpdateNetworkId
#### Used to update the stored network id

    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
       Api->UpdateNetworkId(137);
    }

### Example UpdateProviderUrl
#### Used to update the provider url of the wallet

    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   Api->UpdateProviderURL("NewProviderUrl");
    }

### Indexer & the Wallet

The indexer is tied nicely with the wallet to allow for ease of use.
One thing to note is the NetworkId you set with your wallet is the one that will be used with
the indexer. The default network we set is `137`

## Ping
    
	const TSuccessCallback<bool> GenericSuccess = [=](const bool bSuccess)
	{
        //Ping response is in bSuccess
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//Ping failure
	};

    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   Api->Ping(GenericSuccess, GenericFailure);
    }

## Version

    const TSuccessCallback<FVersion> GenericSuccess = [=](const FVersion& version)
    {
        //Response contained in FVersion
    };

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//Version Failure
	};

    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   Api->Version(GenericSuccess, GenericFailure);
    }

## RunTimeStatus

    const TSuccessCallback<FRuntimeStatus> GenericSuccess = [=](const FRuntimeStatus& runTimeStatus)
    {
        //Response is in FRunTimeStatus
    };

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//RunTimeStatus Failure
	};

    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
       Api->RunTimeStatus(GenericSuccess, GenericFailure);
    }

## GetChainID
    
    const TSuccessCallback<int64> GenericSuccess = [=](const int64 chainID)
	{
        //Response in int64
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//GetChainID Failure
	};
   
    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   Api->GetChainID(GenericSuccess, GenericFailure);
    }

## GetEtherBalance

    const TSuccessCallback<FEtherBalance> GenericSuccess = [=](const FEtherBalance& etherBalance)
	{
        //Response in FEtherBalance
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//GetEtherBalance Failure
	};
    
    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   Api->GetEtherBalance(Api->GetWalletAddress(), GenericSuccess, GenericFailure);
    }

## GetTokenBalances

    const TSuccessCallback<FGetTokenBalancesReturn> GenericSuccess = [=](const FGetTokenBalancesReturn& tokenBalances)
	{
        //Response in FGetTokenBalancesReturn
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//GetTokenBalances Failure
	};
   
    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   FGetTokenBalancesArgs args;
	   args.accountAddress = Api->GetWalletAddress();
	   args.includeMetaData = true;
	   Api->GetTokenBalances(args, GenericSuccess, GenericFailure);
    }

## GetTokenSupplies

    const TSuccessCallback<FGetTokenSuppliesReturn> GenericSuccess = [=](const FGetTokenSuppliesReturn& tokenSupplies)
	{
        //Response is in FGetTokenSuppliesReturn
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//GetTokenSupplies Failure
	};
    
    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   FGetTokenSuppliesArgs args;
	   args.contractAddress = "0x01";//Testing Contract Address in hex with leading 0x
	   args.includeMetaData = true;
	   Api->GetTokenSupplies(args, GenericSuccess, GenericFailure);
    }

## GetTokenSuppliesMap

    const TSuccessCallback<FGetTokenSuppliesMapReturn> GenericSuccess = [=](const FGetTokenSuppliesMapReturn& tokenSuppliesMap)
	{
        //Response is in FGetTokenSuppliesMapReturn
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
        //GetTokenSuppliesMap Failure
	};

    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();    

	   TMap<FString, FTokenList> tokenMap;
	   const TPair<FString,FTokenList> item;
	   tokenMap.Add(item);

       FGetTokenSuppliesMapArgs args;
	   args.includeMetaData = true;
	   args.tokenMap = tokenMap;

	   Api->GetTokenSuppliesMap(args, GenericSuccess, GenericFailure);
    }

## GetBalanceUpdates

    const TSuccessCallback<FGetBalanceUpdatesReturn> GenericSuccess = [=](const FGetBalanceUpdatesReturn& balanceUpdates)
	{
        //Response in FGetBalanceUpdatesReturn
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//GetBalanceUpdates Failure
	};
   
    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   FGetBalanceUpdatesArgs args;
	   args.contractAddress = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	   args.page.page = 10;
	   args.page.more = true;

	   Api->GetBalanceUpdates(args, GenericSuccess, GenericFailure);
    }

## GetTransactionHistory

    const TSuccessCallback<FGetTransactionHistoryReturn> GenericSuccess = [=](const FGetTransactionHistoryReturn& transactionHistory)
	{
        //Response is in FGetTransactionHistoryReturn
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//GetTransactionHistory Failure
	};
   
    const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();

	   FGetTransactionHistoryArgs args;
	   args.filter.accountAddress = Api->GetWalletAddress();
	   args.includeMetaData = true;
	   args.page.page = 0;
	   args.page.more = true;    

	   Api->GetTransactionHistory(args, GenericSuccess, GenericFailure);
    }

***

Assuming you've setup your controlling Pawn with the **[AC_SequencePawn_Component]**
The sequence pawn component has functions to do the following:

Setup Sequence (sets up the sequence based systems), requires playerController input

Show GUI Shows the UI

Hide GUI Hides the UI

GUI Visible Simple Visibility test for the UI

Switch Platform (Switches which mode the UI will be in and how it will be displayed)

Note: this doesn't rotate the application into any one view it just makes the UI responsive to that type of view.

Modes:
- Desktop (default)
- Mobile Portrait (Custom built for portrait mode reducing the X width where ever possible)
- Mobile Landscape

***

#### Unreal TFunctions & Async Best Practices

We make use of TFunctions with some callbacks:

```clike
TSuccessCallback
const TFunction<void(FString)> OnResponse = **[Capturable variables]**(const FString& Response)
{
//callback body where we can process Response
};

FFailureCallback
const TFunction<void(FSequenceError)> OnFailureTest = **[Capturable variables]**(const FSequenceError& Error)
{
//callback body where we can process Error
};
```

One thing to be aware of is keep an eye on capturables if you have lots of nested TFunctions it's very easy to miss
something and start over writing memory. If you require lots of nesting swapping to a better approach using
UFUNCTION callbacks helps to avoid these problems similar to how things are done in **[UAuthenticator.h/cpp]**

### Blockchain Functionality

Most users of the Sequence SDK will not need to interact with cryptographic functions directly.

#### Binary Data

We encapsulate binary data using the ``FBinaryData`` structs, which is a wrapper around a pointer to a shared byte array `TSharedPtr<TArray<uint8>>`.
Binary data is further subtyped into `FUnsizedData`, which represents data of any variable size, and `TSizedData<TSize>`, which represents data of a required byte length `TSize`.

Important cryptographic types of set size, such as 32-byte private keys, are defined as subtypes of ``TSizedData``- for example, we define `FPrivateKey : TSizedData<32>`.
These can also be loaded from hex strings using ``From(FString Str)``, such as ``FPrivateKey::From("0x0...0");``. Ensure that the input string is the correct size.

#### The ABI

To call contracts on the blockchain, you will need to encode any data you wish to pass as arguments using the ABI. To read more about the ABI and its specification, check out the [solidity docs](https://docs.soliditylang.org/en/latest/abi-spec.html).

Our ABI implementation centers around the ``ABI`` class in `ABI/ABI.h`, which provides functions to convert the following types: `UInt32`, `Int32`, `Bool`, `FAddress`, and `FString`. Any other data may be transformed directly into `TFixedABIArray` or `TDynamicABIArray` for fixed-length and dynamic length arrays respectively, or to `TFixedABIData` and `TDynamicABIData` for fixed-length and dynamic length binary data.

Once you have your data stored in `ABIEncodeable` types, you can provide the ABI an array of the type `TArray<ABIEncodeable*>` to `ABI::Encode` to receive the binary encoding of the arguments.
See `TestABI.cpp` for an example.

#### Cryptographic Functions

`Eth/Crypto.h` provides some important ethereum functions for interacting directly with the blockchain:

```
// Derives a public key from a private key
FPublicKey GetPublicKey(FPrivateKey PrivateKey);

// Derives an address from the public key
FAddress GetAddress(FPublicKey PublicKey);

// Finds a keccak hash for some binary data
FHash256 GetKeccakHash(FBinaryData &Data);

// Derives contract address from a given sending address and nonce
FAddress GetContractAddress(FAddress Sender, FBlockNonce Nonce);
```

#### Raw Ethereum Transactions

`EthTransaction.h` contains a struct designed for managing raw ethereum transactions, including functions to sign and hash them. Note that transactions should usually be handled via the Sequence wallet interface, which sends the transactions via the Sequence WAAS.

### Packaging

To set your system up for Packaging please refer to the following links:

- [Windows and macOS](https://dev.epicgames.com/documentation/en-us/unreal-engine/packaging-unreal-engine-projects?application_version=5.3)
- [Setting up Visual Studio for Unreal on Windows](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine)
- [Android](https://dev.epicgames.com/documentation/en-us/unreal-engine/packaging-android-projects-in-unreal-engine?application_version=5.3)
- [iOS](https://dev.epicgames.com/documentation/en-us/unreal-engine/packaging-ios-projects-in-unreal-engine?application_version=5.3)
- [Mac Specific Software Requirements](https://dev.epicgames.com/documentation/en-us/unreal-engine/hardware-and-software-specifications-for-unreal-engine)

#### iOS
For iOS apps you also need to setup provisioning, [following these docs](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-ios-tvos-and-ipados-provisioning-profiles-and-signing-certificates-for-unreal-engine-projects?application_version=5.3)

### Android
When setting up your project to build for Android you'll need to update the following settings:
In ProjectSettings/Android SDK
Set SDK API Level to Android-34
Set NDK API Level to anything in the range [26,33] (We personally used android-32)

### Hardware Requirements
For Hardware Requirements with Unreal please refer to [these docs](https://dev.epicgames.com/documentation/en-us/unreal-engine/hardware-and-software-specifications-for-unreal-engine?application_version=5.3)

#### Unreal and Xcode Specifics
During the Unreal Package process in the event a code signing error occurs you can take the following steps within XCode to get your packaged .app file

1) After packaging the project in Unreal, open the Xcode project (Sequence-unreal folder -> Intermediate -> ProjectFilesIOS -> SequenceUnreal.xcodeproj)
2) Click on the project name on the left hand side to open up project settings
3) Click the Build Phase Tab
4) Click on the ‘+’ icon at the top left
5) Select Run Script
6) Drag the new run script to one below from the last item in the phase list
7) Expand the run script
8) In the script box, add the following command: `xattr -cr /[path-to-your-project]/[your-project-name]/Binaries/IOS/Payload/[your-project-name.app]`
9) Click on the Build Settings tab
10) Click on each item under the Architectures header that contains macOS and hit the delete key
11) Click on the General tab
12) Click on Mac and Applevision Pro under supported destinations and hit the delete key
13) Now the project can be built (if the build fails at first, wait a few moments then try again. It can sometimes take a bit before the build registers the run script)
14) Once you have finished running the project, and want to make changes to the code, REMEMBER to delete this xcodeproj file in the sequence-unreal folder to ensure that a new xcodeproj is creating when you packaging the project again._
