# Sequence Unreal SDK
This SDK allows easy integration of Sequence Embedded Wallet from any Unreal Framework project.

## Documentation
https://docs.sequence.xyz/sdk/unreal/overview

## Requirements
Unreal 5.2*, 5.3*, and 5.4 (* Supported by Beta 1.0.3 and lower)

## Supported Platforms

- Android
- iOS
- PC standalone
- Mac standalone

This plugin requires the modern xcode setting to be enabled to build on MacOS.
See [the following link](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-modern-xcode-in-unreal-engine-5.3-and-newer) for more details.

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
      WaaSConfigKey = ""
      ProjectAccessKey = ""
      GoogleClientID = ""
      AppleClientID = ""
      FacebookClientID = ""
      DiscordClientID = ""
      RedirectUrl = "https://api.sequence.app"
      PlayFabTitleID = ""

Here is where you'll fill in the various configuration values for the plugin.
For the time being we don't support Facebook or Discord authentication so feel free to ignore those 2 clientId's for now.

### Note when upgrading from older versions of the plugin
The WaaSTenantKey value in the SequenceConfig.ini has been changed to WaaSConfigKey
if you do not update this value in the SequenceConfig.ini, the SequencePlugin will be unable
to load your settings.

## Installing Sequence from EpicGame Store
If you are installing the plugin from the Epic game store the process is slightly different.
First you'll actually want to install the plugin into the engine from the store. Once this is done
you'll open up a project, Go to Edit/Plugins/Sequence/SequencePlugin. You'll need to enable the plugin
as it won't be enabled by default. The engine will need to restart before you'll be able to see the plugin content.

The second thing you'll need to be aware of is the location of the SequencePluginContent folder. Because it's
installed as an engine plugin, you'll need to enable engine content in the content drawer to be able to find it.
The location will be All/Engine/Plugins/SequencePluginContent/Core, within the content drawer.

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

- `WaaSConfigKey`
- `ProjectAccessKey`

You can then add these credentials in the **[SequenceConfig.ini]** file under [YourProject]/Config/SequenceConfig.ini

Before you can use PlayFab you'll need to create a game with PlayFab [Here](https://developer.playfab.com/)
Then in the SequencePluginConfig.ini, set the PlayFabTitleID String to the TitleID of your game.

## Security

You must provide a 32 character encryption key in the **[SequenceConfig.ini]** file under [YourProject]/Config/SequenceConfig.ini
under the config variable `FallbackEncryptionKey`

In order to prevent tampering with data you must encrypt your packaged project using Unreals packaging settings
You can refer to [these docs](https://dev.epicgames.com/documentation/en-us/unreal-engine/packaging-unreal-engine-projects?application_version=5.3)

Note: For generating secure keys feel free to use tools such as:
Lastpass Password generator or equivalent,
Encryption Key Generators,
etc

##### Important for values you place in FallbackEncryptionKey
Be sure what ever key data you place in **[FallbackEncryptionKey]** is of hex form & does NOT contain any extraneous symbols, eg) *,/\ etc.
In the event unrecognized symbols are seen the engine will not load the .ini file.

***

## Getting Started With the Builtin GUI

1) Once you have the `SequencePlugin` folder, you'll need to go to your project directory and create a `Plugins` folder in it,
   then copy over the `SequencePlugin` folder into the `Plugins` folder. If a `Plugins` folder already exists just copy the `SequencePlugin` folder into it.

2) Launch your project, then allow it to update the UProject Settings.

3) To find the `SequencePlugin` content folder in your content drawer enable view plugin content

4) If you wish to use the in built sequence UI for login you have to do the following:

### For Blueprints

1) Create a blueprint Actor (if you wish to spawn it yourself) Or Pawn (if you wish to use it with a Gamemode)

2) Attach the **[AC_SequencePawn_Component]** to the Blueprint you created via the add components section.

[Where to find Components window](ReadmeImages/Example_AddComponent.PNG)

3) Then you can setup your blueprint like so to start utilizing the SequenceAPI

[How to setup the Blueprint](ReadmeImages/Example_BP_Setup.PNG)

Note: Auth Success Forwarder will let you know when the system is ready to be used

4) Depending on what you chose your blueprint parent class to be, You can do one of two things to finish this
   process. If it's a pawn or a subclass of a pawn, you can attach it to your Gamemode so that it spawns when play begins, OR you can drag it out into
   your scene if it's just an actor Blueprint.

### For C++

1) Create a C++ Class that Inherits from **[Pawn]** (If you wish to use it with Gamemode) or **[Actor]** (If you'll be spawning it yourself)
   If you don't know how to do this refer to the doc [Creating C++ Classes in Unreal](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-the-cplusplus-class-wizard-in-unreal-engine?application_version=5.2),
   for the purpose of these docs I'll refer to the C++ Class created here as the **[C++ Parent]**

2) In **[C++ Parent]** .h file include the Header **[SequenceAPI.h]** this will allow you to access the **[USequenceWallet]**

3) Create a BlueprintCallable function within the **[C++ Parent]**

4) Create a Blueprint that inherits from **[C++ Parent]**, Then Attach the following Actor component to it **[AC_SequencePawn_Component]**. For in depth specifics on how to setup this blueprint
   please refer to the demonstration BP Graph [Image](ReadmeImages/Example_BP_Setup.PNG), this is the BP Graph of **[BP_CustomSpectatorPawn]** contained within the plugins content folder, & serves as a template for your
   own Blueprint graph.

-  For those who aren't familiar with Unreal's Blueprint system you can create a blueprint by right clicking in the content
   drawer, then click blueprint class. Within the blueprint class selector select the All Classes dropdown & search  
   for your **[C++ Parent]** class you just made.

Note: You can simply duplicate the **[BP_CustomSpectatorPawn]** (if you do this be sure to move the duplicate outside
of the plugin folder into YOUR content folder, Otherwise your work could be lost during an update to the plugin).

5) If your **[C++ Parent]** was a pawn, you can set it to be the default pawn in your Gamemode and it will spawn on BeginPlay,
   If your **[C++ Parent]** was an Actor, you can manually add it to the scene.
   In both cases the UI will show up on BeginPlay.

Note: If you don't know how to modify / update the Gamemode / Gamemode settings go to ProjectSettings -> Maps & Modes, From there
you can set the Gamemode and update the default pawn. Or in the case you wish to use our Gamemode for testing it's
**[GM_Sequence]** You'll just need to set your pawn as the default pawn.

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

In a C++ UObject with a series of pass through **[UFUNCTIONS]** setup similarly to **[SequenceBackendManager.h/.cpp]**. Each of these calls is implemented in **[UAuthenticator]** you just need to pass through the data with YOUR UAuthenticator UObject

```clike

	/**
	 * Resets the IsFederatingSessionInUse State to false
	 * For cases where you don't want to Federate a session in use if the error
	 * occurs for "EmailAlreadyInUse"
	 */
	void ResetFederateSessionInUse();
	
	/**
	 * Sets a custom encryptor
	 * @param EncryptorIn Encryptor to use
	 */
	void SetCustomEncryptor(UGenericNativeEncryptor * EncryptorIn);

	/**
	 * Used to get an OIDC Login Url
	 * @param Type Type of OIDC Url need
	 * @return An OIDC login Url of the specified Type
	 */
	FString GetSigninURL(const ESocialSigninType& Type) const;

	/**
	 * Used to initiate mobile Login
	 * @param Type Type of OIDC to conduct on Mobile
	 * @param ForceCreateAccountIn Force create account if it already exists
	 */
	void InitiateMobileSSO(const ESocialSigninType& Type, const bool ForceCreateAccountIn);

	/**
	 * Internal Mobile Login call. Used to complete mobile login once a tokenized URL is received
	 * @param TokenizedUrl The URL containing an IdToken
	 */
	void UpdateMobileLogin(const FString& TokenizedUrl);

	/**
	 * Internal Mobile Login Call. Used to complete mobile Login once an IdToken has been received
	 * @param IdTokenIn IdToken Received from mobile login
	 */
	void UpdateMobileLogin_IdToken(const FString& IdTokenIn);

	/**
	 * Used to initiate OIDC login
	 * @param IDTokenIn OIDC Token granted from login
	 * @param ForceCreateAccountIn Force create account if it already exists
	 */
	void SocialLogin(const FString& IDTokenIn, const bool ForceCreateAccountIn);

	/**
	 * Used to initiate email login
	 * @param EmailIn Email
	 * @param ForceCreateAccountIn Force create account if it already exists
	 */
	void EmailLogin(const FString& EmailIn, const bool ForceCreateAccountIn);

	/**
	 * Used to login as a Guest into Sequence
	 * @param ForceCreateAccountIn Force create account if it already exists
	 */
	void GuestLogin(const bool ForceCreateAccountIn) const;

	/**
	 * Used to create & login a new account with PlayFab, Then OpenSession with Sequence
	 * @param UsernameIn Username
	 * @param EmailIn Email
	 * @param PasswordIn Password
	 * @param ForceCreateAccountIn Force create account if it already exists
	 */
	void PlayFabRegisterAndLogin(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const bool ForceCreateAccountIn);

	/**
	 * Used to login with PlayFab, Then OpenSession with Sequence
	 * @param UsernameIn Username
	 * @param PasswordIn Password
	 * @param ForceCreateAccountIn Force create account if it already exists
	 */
	void PlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, const bool ForceCreateAccountIn);

   /**
     * Used to authenticate using a Session Ticket
     * @param SessionTicket Session Ticket to login with
     */
    void PlayfabAuthenticateWithSessionTicket(const FString& SessionTicket);

	/**
	 * Used to complete Email based authentication, whether it be for normal Authentication OR Federation
	 * @param CodeIn Received Code from email
	 */
	void EmailLoginCode(const FString& CodeIn);

	/**
	 * Used To Federate an Email (WIP)
	 * @param EmailIn Email to federate
	 */
	void FederateEmail(const FString& EmailIn);

	/**
	 * Used to Federate an OIDC Login
	 * @param IdTokenIn OIDC Token To federate
	 */
	void FederateOIDCIdToken(const FString& IdTokenIn);

	/**
	 * Used to initiate OIDC account federation on mobile
	 * @param Type Type of OIDC account to federate
	 */
	void InitiateMobileFederateOIDC(const ESocialSigninType& Type);

	/**
	 * Used to federate a new PlayFab account
	 * @param UsernameIn PlayFab Username
	 * @param EmailIn PlayFab Email
	 * @param PasswordIn PlayFab Password
	 */
	void FederatePlayFabNewAccount(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn) const;

	/**
	 * Used to federate an existing account on PlayFab
	 * @param UsernameIn PlayFab Username
	 * @param PasswordIn PlayFab Password
	 */
	void FederatePlayFabLogin(const FString& UsernameIn, const FString& PasswordIn) const;

	/**
	 * Used to force open the last failed OpenSession Attempt
	 */
	void ForceOpenLastOpenSessionAttempt() const;

	/**
	 * Used to get stored credentials from Disk
	 * @return Stored Credentials
	 */
	FStoredCredentials_BE GetStoredCredentials() const;

	/**
	 * Used to store Credentials on Disk
	 * @param Credentials Credentials to be Stored
	 */
	void StoreCredentials(const FCredentials_BE& Credentials) const;

	/**
	 * Clears stored credentials on disk with blanks
	 */
	void ClearStoredCredentials() const;

```

To start you'll want to create a **[UAuthenticator]** UObject like so **[UAuthenticator * Auth = NewObject<UAuthenticator>()]**, this UObject manages the authentication side of Sequence.

Be sure to bind to the Delegates for **[AuthSuccess]**, **[AuthFailure]**, **[AuthRequiresCode]** prior to making any signin calls You can bind to these delegates like so:

```clike
this->Authenticator->AuthSuccess.AddDynamic(this, &ASequenceBackendManager::CallShowAuthSuccessScreen);
this->Authenticator->AuthRequiresCode.AddDynamic(this, &ASequenceBackendManager::CallReadyToReceiveCode);
this->Authenticator->AuthFailure.AddDynamic(this, &ASequenceBackendManager::CallShowAuthFailureScreen);
this->Authenticator->FederateSuccess.AddDynamic(this, &ASequenceBackendManager::CallShowFederationSuccess);
this->Authenticator->FederateFailure.AddDynamic(this, &ASequenceBackendManager::CallShowFederationFailure);
this->Authenticator->FederateOrForce.AddDynamic(this, &ASequenceBackendManager::CallShowFederateOrForce);
```

Note: Replace the usage of the SequenceBackendManager.h/.cpp with you're own when building a custom GUI,
it is only used here as a reference in the event more context is needed with these instructions.
Where **[CallShowAuthSuccessScreen]** is defined in `SequenceBackendManager.h` as an example like so:

```clike
UFUNCTION()
void CallShowAuthSuccessScreen();
```

And in `SequenceBackendManager.cpp` like so:

```clike
void ASequenceBackendManager::CallShowAuthSuccessScreen() 
{
this->Credentials = CredentialsIn;
if (this->ShowAuthSuccessDelegate.IsBound())
  this->ShowAuthSuccessDelegate.Broadcast();
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

### PlayFab Social Signin based Authentication with CustomUI
1) Start by calling either PlayFabLogin (Login With Existing)
   or PlayFabRegisterAndLogin (Create a new PlayFab account & Login with it) that's it.
   or PlayfabAuthenticateWithSessionTicket (Authenticate with a PlayFab Session Ticket)

### Guest Login with CustomUI
1) Start by calling GuestLogin, and that's it.

### Account Federation

In cases where users sign in & all is well, you can allow users to federate other login types so long as the Email matches.
Doing this allows users to login with Email, OIDC & Playfab login types such that they all access the same wallet address.

You can use the following calls to achieve this:
"UAuthenticator::FederateEmail" , "UAuthenticator::FederateOIDCIdToken" , "UAuthenticator::InitiateMobileFederateOIDC" ,
"UAuthenticator::FederatePlayFabNewAccount" , "UAuthenticator::FederatePlayFabLogin"

In the case of FederateEmail you need to be bound to the AuthRequiresCode Delegate and you complete the call with
"UAuthenticator::EmailLoginCode"

### Account Federation & Force Create Account for EmailAlreadyInUse Cases

In cases where users attempt to signin but are already logged into another session with the same Email, you'll need to do
the following.

First be sure the following delegates are bound: FederateSuccess , FederateFailure , FederateOrForce

FederateSuccess Fires when the Federation Operation Is Successful.

FederateFailure Fires when the Federation Operation Fails, It also includes the string Error of what went wrong.

FederateOrForce Fires when an Authentication attempt fails with EmailAlreadyInUse error.

When a user attempts to Signin and it fails with email already in use, FederateOrForce will fire. This delegate
will include *[FFederationSupportData]*, which contains 2 pieces of information,
The email the user wishes to federate & a list of login types they are already signed in with.
We have 2 options we can choose between:

1) Federate: With the ValidLoginTypes list, Present those options to the user as login methods to use, Being sure to specify that they
   must use the email that was also presented in *[FFederationSupportData]*. If they successfully login with one of those types,
   the system will automatically federate their account and no further action will be required.

2) ForceCreate: Ask the user if they'd wish to force create a new account with the email address present in *[FFederationSupportData]*
   & login type they initially tried.
   This will assign a new wallet address to them & it will be treated like an entirely separate account.
   To do this simply call "UAuthenticator::ForceOpenLastOpenSessionAttempt" This will ForceCreate a new account with the
   last login attempt that resulted in an EmailAlreadyInUse error.

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

### Blueprint Sequence API

In order to use the Blueprint Sequence API, you must create an actor or pawn and attach the **[AC_SequencePawn_Component]**

#### Sequence API

###### Sequence Utils

###### Human Readable Values to Transaction Readable Values
When setting values for ERC20 amounts you may try to set a value like 0.15 USDC in the ValueString of the FERC20Transaction struct,
However this is NOT the value you put inside the Value String for ERC20 Transactions, The value you put in is 0.15 * 10^6. To simplify this process
there are Util functions in Blueprints that you can use to put human readable values in like so.

[Example Conversion](ReadmeImages/BP_Demo/SequenceApi/Util/Example_HumanReadable_To_Transaction_Readable.PNG)

Where the TokenBalance Structure is the same type that's contained in the return from the Indexer Call GetTokenBalances

[Example TokenBalance](ReadmeImages/BP_Demo/SequenceApi/Util/Example_TokenBalance.PNG)

In some cases the TokenBalance Struct won't have a decimals Value initiated properly. In which case you can actually hard code
the decimals value with the following conversion function instead.

[Example Hardcode](ReadmeImages/BP_Demo/SequenceApi/Util/Example_Hardcode.PNG)

##### Get Wallet Address , Type Sync

[Get Wallet Address](ReadmeImages/BP_Demo/SequenceApi/Sync/Example_Get_Wallet_Address.PNG)

##### Get Currently Set Network Id , Type Sync

[Get Network Id](ReadmeImages/BP_Demo/SequenceApi/Sync/Example_Get_Network_Id.PNG)

##### Update Currently Set Network Id , Type Sync

[Update Network Id](ReadmeImages/BP_Demo/SequenceApi/Sync/Example_Update_Network_Id.PNG)

#### Get All Networks , Type Sync

[Get All Networks](ReadmeImages/BP_Demo/SequenceApi/Sync/Example_GetAllNetworks.PNG)

#### Get All Network Names , Type Sync

[Get All Networks](ReadmeImages/BP_Demo/SequenceApi/Sync/Example_GetAllNetworkNames.PNG)

#### Get All Network Ids , Type Sync

[Get All Networks](ReadmeImages/BP_Demo/SequenceApi/Sync/Example_GetAllNetworkIds.PNG)

#### Get Network Id , Type Sync

[Get All Networks](ReadmeImages/BP_Demo/SequenceApi/Sync/Example_GetNetworkId.PNG)

#### Get Network Name , Type Sync

[Get All Networks](ReadmeImages/BP_Demo/SequenceApi/Sync/Example_GetNetworkName.PNG)

##### Open Transak Url , Type Sync

[Open Transak Url](ReadmeImages/BP_Demo/SequenceApi/Sync/Example_Open_Transak_Url.PNG)

##### SignOut , Type Sync

[SignOut](ReadmeImages/BP_Demo/SequenceApi/Sync/Example_Signout.PNG)

##### Sign Message , Type Async

[Sign Message](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_SignMessage.PNG)

##### The following 4 calls use a Transactions Object
You can add the various transaction types to a Transactions UObject like so in blueprints.

###### ERC20

[Creating a Transactions Object & a ERC20 Type to it](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_Add_ERC20.PNG)
[Where the DemoERC20 struct looks like](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_ERC20.PNG)

###### ERC721

[Creating a Transactions Object & a ERC721 Type to it](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_Add_ERC721.PNG)
[Where the DemoERC721 struct looks like](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_ERC721.PNG)

###### ERC1155

[Creating a Transactions Object & a ERC1155 Type to it](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_Add_ERC1155.PNG)
[Where the DemoERC1155 struct looks like](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_ERC1155.PNG)

###### Raw

[Creating a Transactions Object & a Raw Type to it](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_Add_Raw.PNG)
[Where the DemoRaw struct looks like](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_Raw.PNG)

###### DelayedEncoded

[Creating a Transactions Object & a DelayedEncoding Type to it](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_Delayed_Pt1.PNG)

[You can go about creating the Arguments object like so](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_DelayedEncode_Pt2.PNG)

##### Get Filtered Fee Options , Type Async

[Get Filtered Fees](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_GetFilteredFees.PNG)

##### Get Unfiltered Fee Options , Type Async

[Get UnFiltered Fees](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_GetUnfilteredFees.PNG)

##### Send Transaction With Fee , Type Async

[Send Transaction with Fee](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_SendTransactionWithFee.PNG)

##### Send Transaction , Type Async

[Send Transaction](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_SendTransaction.PNG)

##### List Sessions , Type Async

[List Sessions](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_ListSessions.PNG)

##### Get Supported Transak Countries , Type Async

[Get Supported Transak Countries](ReadmeImages/BP_Demo/SequenceApi/ASync/Example_GetSupportedTransakCountries.PNG)

#### Indexer

##### Ping , Type Async

[Indexer Ping](ReadmeImages/BP_Demo/Indexer/Example_Ping.PNG)

##### Version , Type Async

[Indexer Version](ReadmeImages/BP_Demo/Indexer/Example_Version.PNG)

##### RunTime Status , Type Async

[Indexer RuntimeStatus](ReadmeImages/BP_Demo/Indexer/Example_RuntimeStatus.PNG)

##### Get Chain Id , Type Async

[Indexer GetChainId](ReadmeImages/BP_Demo/Indexer/Example_GetChainId.PNG)

##### Get Ether Balance , Type Async

[Indexer GetEtherBalance](ReadmeImages/BP_Demo/Indexer/Example_GetEtherBalance.PNG)

##### Get Token Balances , Type Async

[Indexer GetTokenBalances](ReadmeImages/BP_Demo/Indexer/Example_GetTokenBalances.PNG)

##### Get Token Supplies , Type Async

[Indexer GetTokenSupplies](ReadmeImages/BP_Demo/Indexer/Example_GetTokenSupplies.PNG)

##### Get Token Supplies Map , Type Async

[Indexer GetTokenSuppliesMap](ReadmeImages/BP_Demo/Indexer/Example_GetTokenSuppliesMap.PNG)

##### Get Balance Updates , Type Async

[Indexer GetBalanceUpdates](ReadmeImages/BP_Demo/Indexer/Example_GetBalanceUpdates.PNG)

##### Get Transaction History , Type Async

[Indexer GetTransactionHistory](ReadmeImages/BP_Demo/Indexer/Example_GetTransactionHistory.PNG)

### C++ Sequence API

### Updates to C++ Sequence API

We have created a wrapper TypeDef for TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction, FDelayedTransaction>
called TransactionUnion, You should use the TransactionUnion DataType instead. This change also comes as we have introduced the Delayed Transaction Type.

### Using the C++ Sequence API

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

### Example GetSystemReadableAmount
#### Used to convert a user readable amount of a token into a system usable one, IE) 1 USDC -> 1000000 (int64 representation of 1 USDC)

      int64 SystemReadableAmount = USequenceWallet::GetSystemReadableAmount(0.01, 6);//0.01 USDC

### Example GetUserReadableAmount
#### Used to convert a system readable amount of a token into a user readable one IE) 1000000 (int64 representation of 1 USDC) -> 1 USDC

      float UserReadableAmount = USequenceWallet::GetUserReadableAmount(1000000, 6);//1 USDC

### Example GetFeeOptions
#### Used to get Filtered Fee Options (that is options that your wallet can pay)

   	const TFunction<void(TArray<FFeeOption>)> OnResponse = [=](const TArray<FFeeOption>& Response)
	{
      //Process filtered fee options
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		//Process Failure state
	};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		TArray<TransactionUnion> Transactions;
		FERC20Transaction T20;
		T20.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
		T20.tokenAddress = "0x2791Bca1f2de4661ED88A30C99A7a9449Aa84174";
		T20.value = "1000";
		Transactions.Push(TransactionUnion(T20));
		Api->GetFeeOptions(Transactions,OnResponse,GenericFailure);
	}

### Example GetUnfilteredFeeOptions
#### Used to get all FeeOptions for a given transaction

	const TFunction<void(TArray<FFeeOption>)> OnResponse = [=](const TArray<FFeeOption>& Response)
	{
      //Process fee options
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
      //Process Failure state
	};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		TArray<TransactionUnion> Transactions;
		FERC20Transaction T20;
		T20.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
		T20.tokenAddress = "0x2791Bca1f2de4661ED88A30C99A7a9449Aa84174";
		T20.value = "1000";
		Transactions.Push(TransactionUnion(T20));
		Api->GetUnfilteredFeeOptions(Transactions,OnResponse,GenericFailure);
	}

### Example SendTransactionWithFeeOption
#### Used to get a list of fee's and send a transaction with one of them valid ones

	TArray<TransactionUnion> Transactions;
	FERC721Transaction T721;
	T721.safe = true;
	T721.id = "101424663676543340124133645812717438592241191887187111290563634379068086785120";
	T721.to = "0x245b738089F1fa668D927422d2943F75A9e89819";
	T721.tokenAddress = "0xa9a6a3626993d487d2dbda3173cf58ca1a9d9e9f";
	Transactions.Push(TransactionUnion(T721));
	
	const TFunction<void(TArray<FFeeOption>)> OnFeeResponse = [Transactions, OnSuccess, OnFailure](const TArray<FFeeOption>& Response)
	{
		if (Response.Num() > 0)
		{
			const FFeeOption SelectedFeeOption = Response[0];
			
			const FFailureCallback OnTransactionFailure = [OnFailure](const FSequenceError& Error)
			{
				OnFailure("Transaction failure", Error);
			};

			const UAuthenticator * Auth = NewObject<UAuthenticator>();
			const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
			if (WalletOptional.IsSet() && WalletOptional.GetValue())
			{
				USequenceWallet * Api = WalletOptional.GetValue();
				Api->SendTransactionWithFeeOption(Transactions,SelectedFeeOption,[=](const FTransactionResponse& Transaction)
				{
					FString OutputString;
					const TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
					FJsonSerializer::Serialize(Transaction.Json.ToSharedRef(), Writer);
					OnSuccess(OutputString);
				}, OnTransactionFailure);
			}
		}
		else
		{
			OnFailure("Test failed no fee options in response",FSequenceError(EErrorType::EmptyResponse,"Empty fee option response"));
		}
	};

	const FFailureCallback OnFeeFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Get Fee Option Response failure", Error);
	};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->GetFeeOptions(Transactions,OnFeeResponse,OnFeeFailure);
	}

### GetSupportedCountries
#### Used to get a list of supported countries and requirements for transak

   	const TFunction<void (TArray<FSupportedCountry>)> OnSuccess = [=](TArray<FSupportedCountry> SupportedCountries)
	{
      //Process success
	};

	const TFunction<void (FSequenceError)> OnFailure = [=]( const FSequenceError& Err)
	{
		//Process error
	};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->GetSupportedTransakCountries(OnSuccess,OnFailure);
	}

### Example LoadTransakURL
#### Used to open a TransakURL in an external browser

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->OpenTransakLink();
	}

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

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
       const FString Message = "Hi";
       Api->SignMessage(Message,OnResponse,OnFailure);
    }

### Example SendTransaction
##### Used to send a transaction / perform contract calls
Note: if you want call contracts with the Raw type you'll want to include the header
`#include "ABI/ABI.h"` in order to use the ABI to encode the data for a contract call.

	const FFailureCallback OnFailure = [=](const FSequenceError& Error)
	{
		UE_LOG(LogTemp,Display,TEXT("Error Message: %s"),*Error.Message);
	};
	
    //Create the Transaction object list
	TArray<TransactionUnion> Txn;
	
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
    
	Txn.Push(TransactionUnion(T20));//ERC20
	Txn.Push(TransactionUnion(T721));//ERC721
	Txn.Push(TransactionUnion(T1155));//ERC1155
	Txn.Push(TransactionUnion(T));//ContractCall

    //Now send the transaction
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
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

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
	   USequenceWallet * Api = WalletOptional.GetValue();
       Api->ListSessions(OnSuccess,OnFailure);
    }

### Example SignOut
##### Closes the session & clears out cached credentials with blank ones

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
	   USequenceWallet * Api = WalletOptional.GetValue();
       Api->SignOut();
    }

### Example GetWalletAddress
#### Gets the wallet address currently being used

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
	   USequenceWallet * Api = WalletOptional.GetValue();
       Api->GetWalletAddress();
    }

### Example GetNetworkId
#### Gets the network id being used

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
       Api->GetNetworkId();
    }

### Example UpdateNetworkId
#### Used to update the stored network id

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
       Api->UpdateNetworkId(137);
    }

### Example GetAllNetworks
#### Used to get all available networks as structs of their names & ids

	TArray<FIdNamePair> Networks = USequenceWallet::GetAllNetworks();

### Example GetAllNetworkNames
#### Used to get all available network names

	TArray<FString> NetworkNames = USequenceWallet::GetAllNetworkNames();

### Example GetAllNetworkIds
#### Used to get all available network Ids

	TArray<int64> NetworkIds = USequenceWallet::GetAllNetworkIds();

### Example GetNetworkId
#### Used to get the network Id associated with the given name

	int64 NetworkId = USequenceWallet::GetNetworkId(TEXT("polygon"));

### Example GetNetworkName
#### Used to get the network name associated with the id

	FString NetworkName = USequenceWallet::GetNetworkName(137);

### Example UpdateProviderUrl
#### Used to update the provider url of the wallet

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   Api->UpdateProviderURL("NewProviderUrl");
    }

## Using Delayed Encoding
The Delayed encoding TransactionType is added onto a TArray<TransactionUnion> like any other type,
However the way this transaction is built is very different from the others.
This type is meant to allow you to send unencoded data to the server for it to encode for you into valid ethereum transaction.

There are 4 DataTypes you need to know about in order to use DelayedEncoding,

1) FDelayedTransaction, This Data type houses all of the arguments for the encoding.
2) UDelayedEncodingBP, This Object is used to represent the Data Portion of the FDelayedTransaction.
3) UDelayedEncodingArrayArgsBP, This is one of two argument UObjects, This one is used for housing Array Args
4) UDelayedEncodingObjectArgsBP, This is the other argument UObject, This one is used for housing Object Args.

The 2 Argument UObjects support full nesting, Meaning you can have an ArrayArg inside an ArrayArg, As well as an ObjectArg
inside an ObjectArg. They also support nesting ObjectArgs in ArrayArgs and ArrayArgs in ObjectArgs. As well as the traditional
JSON Primitives, Such as Bool, Number(float, double, int32, int64), String.

Below is an example of how to Construct an FDelayedTransaction with lots of nesting.

	TArray<TransactionUnion> Txn;
	FDelayedTransaction TDelayed;

	TDelayed.data->SetFunc("setInt()");
	TDelayed.data->SetAbi("Epic ABI");

	UDelayedEncodingObjectArgsBP * NestedObj = NewObject<UDelayedEncodingObjectArgsBP>();
	NestedObj->AddStringArg(TEXT("Deep String"), TEXT("Blah"));
	
	UDelayedEncodingArrayArgsBP * NestedArg = NewObject<UDelayedEncodingArrayArgsBP>();
	NestedArg->AddObjectArg(NestedObj);

	UDelayedEncodingArrayArgsBP * ArgsListInner = NewObject<UDelayedEncodingArrayArgsBP>();
	ArgsListInner->AddBoolArg(false);
	ArgsListInner->AddInt32Arg(32);
	ArgsListInner->AddInt64Arg(64);
	ArgsListInner->AddStringArg(TEXT("String Arg"));
	ArgsListInner->AddArrayArg(NestedArg);
	
	UDelayedEncodingObjectArgsBP * ArgsMain = NewObject<UDelayedEncodingObjectArgsBP>();
	ArgsMain->AddBoolArg(TEXT("Epic Boolean"), false);
	ArgsMain->AddDoubleArg(TEXT("Epic Double Arg"), 0.1);
	ArgsMain->AddInt64Arg(TEXT("Epic integer64 arg"), -1);
	ArgsMain->AddArrayArg(TEXT("List Arg"),ArgsListInner);
	
	TDelayed.data->SetArgs(ArgsMain);
	TDelayed.to = "0x245b738089F1fa668D927422d2943F75A9e89819";
	TDelayed.value = "0";
	
	Txn.Push(TransactionUnion(TDelayed));

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

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   Api->Ping(GenericSuccess, GenericFailure);
    }

## Version

    const TSuccessCallback<FSeqVersion> GenericSuccess = [=](const FSeqVersion& version)
    {
        //Response contained in FSeqVersion
    };

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//Version Failure
	};

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   Api->Version(GenericSuccess, GenericFailure);
    }

## RunTimeStatus

    const TSuccessCallback<FSeqRuntimeStatus> GenericSuccess = [=](const FSeqRuntimeStatus& runTimeStatus)
    {
        //Response is in FSeqRunTimeStatus
    };

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//RunTimeStatus Failure
	};

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
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

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   Api->GetChainID(GenericSuccess, GenericFailure);
    }

## GetEtherBalance

    const TSuccessCallback<FSeqEtherBalance> GenericSuccess = [=](const FSeqEtherBalance& etherBalance)
	{
        //Response in FSeqEtherBalance
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//GetEtherBalance Failure
	};

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   Api->GetEtherBalance(Api->GetWalletAddress(), GenericSuccess, GenericFailure);
    }

## GetTokenBalances

    const TSuccessCallback<FSeqGetTokenBalancesReturn> GenericSuccess = [=](const FSeqGetTokenBalancesReturn& tokenBalances)
	{
        //Response in FSeqGetTokenBalancesReturn
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//GetTokenBalances Failure
	};

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   FSeqGetTokenBalancesArgs args;
	   args.accountAddress = Api->GetWalletAddress();
	   args.includeMetaData = true;
	   Api->GetTokenBalances(args, GenericSuccess, GenericFailure);
    }

## GetTokenSupplies

    const TSuccessCallback<FSeqGetTokenSuppliesReturn> GenericSuccess = [=](const FSeqGetTokenSuppliesReturn& tokenSupplies)
	{
        //Response is in FSeqGetTokenSuppliesReturn
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//GetTokenSupplies Failure
	};

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   FSeqGetTokenSuppliesArgs args;
	   args.contractAddress = "0x01";//Testing Contract Address in hex with leading 0x
	   args.includeMetaData = true;
	   Api->GetTokenSupplies(args, GenericSuccess, GenericFailure);
    }

## GetTokenSuppliesMap

    const TSuccessCallback<FSeqGetTokenSuppliesMapReturn> GenericSuccess = [=](const FSeqGetTokenSuppliesMapReturn& tokenSuppliesMap)
	{
        //Response is in FSeqGetTokenSuppliesMapReturn
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
        //GetTokenSuppliesMap Failure
	};

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();    

	   TMap<FString, FSeqTokenList> tokenMap;
	   const TPair<FString,FSeqTokenList> item;
	   tokenMap.Add(item);

       FSeqGetTokenSuppliesMapArgs args;
	   args.includeMetaData = true;
	   args.tokenMap = tokenMap;

	   Api->GetTokenSuppliesMap(args, GenericSuccess, GenericFailure);
    }

## GetBalanceUpdates

    const TSuccessCallback<FSeqGetBalanceUpdatesReturn> GenericSuccess = [=](const FSeqGetBalanceUpdatesReturn& balanceUpdates)
	{
        //Response in FSeqGetBalanceUpdatesReturn
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//GetBalanceUpdates Failure
	};

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();
	   FSeqGetBalanceUpdatesArgs args;
	   args.contractAddress = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	   args.page.page = 10;
	   args.page.more = true;

	   Api->GetBalanceUpdates(args, GenericSuccess, GenericFailure);
    }

## GetTransactionHistory

    const TSuccessCallback<FSeqGetTransactionHistoryReturn> GenericSuccess = [=](const FSeqGetTransactionHistoryReturn& transactionHistory)
	{
        //Response is in FSeqGetTransactionHistoryReturn
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//GetTransactionHistory Failure
	};

    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
       USequenceWallet * Api = WalletOptional.GetValue();

	   FSeqGetTransactionHistoryArgs args;
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
- Mobile Portrait (Custom built for portrait mode reducing the X width wherever possible)
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

One thing to be aware of is to keep an eye on capturables if you have lots of nested TFunctions it's very easy to miss
something and start over writing memory. If you require lots of nesting swapping to a better approach using
UFUNCTION callbacks helps to avoid these problems.

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

#### SequencePlugin Login Notes
During login on OSX it's likely users will be asked to login to their keychain if they aren't already.
This is because we store a random encryption key in the Users keychain and it may require permission from the user to do so.

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
14) Once you have finished running the project, and want to make changes to the code, REMEMBER to delete this xcodeproj file in the sequence-unreal folder to ensure that a new xcodeproj is created when you package the project again._

## TroubleShooting

In events where the editor crashes on launch, try deleting the following file, Saved/SaveGames/Cr.sav
This file contains the savegame data for credentials and if modified can result in an engine crash.
Deleting this file deletes local credentials, so you'll need to log back in.
