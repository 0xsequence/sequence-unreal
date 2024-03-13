Sequence Unreal SDK
===================

This SDK allows easy integration of Sequence Embedded Wallet from any Unreal Framework project.

## Credentials

Before you can use this plugin, you need to acquire the following credentials from Sequence:

- `WaaSTenantKey`
- `ProjectAccessKey`

You can then add these credentials in the **[Config.h]** file under `SequencePlugin/Source/SequencePlugin/Private/Config/Config.h`.


## Security

You must provide an encryption key implementation at **[SequenceEncryptor.cpp]** function **[GetStoredKey]**. This function must be implemented to provide a securely stored private key that will be used to encrypt and decrypt client information. Failure to do so will result in NO information being stored or in the event you do not use a securely stored key, can result in client information being stored insecurely on their systems.

You have two options for a secure key implementation:

1) You can provide a cryptographically secure key of 32 characters in length and configure your packaged project to be encrypted
   so that the key provided in the function cannot be retrieved from your package.

2) You can use platform specific key stores to securely store your keys. This is the preferred method.

If this implementation is not done properly, user data stored locally can be tampered with and stolen.

***

## Getting Started

1) Once you have the the `SequencePlugin` folder, you'll need to go to your project directory and create a `Plugins` folder in it, then copy over the `SequencePlugin` folder into the `Plugins` folder. If a `Plugins` folder already exists just copy the `SequencePlugin` folder into it.

2) Launch your project, then allow it to update the UProject Settings.

3) To find the `SequencePlugin` content folder in your content drawer enable view plugin content

4) If you wish to use the in built sequence UI for login
    a) Create an **[Actor]** you wish to be responsible for the SequenceUI then attach the **[Sequence_Pawn_Component_BP]** to it
    b) Setup your **[Actor]** Blueprint similar to how it's setup in **[Custom_Spectator_Pawn]** being sure to bind to the delegate that gives you Credentials **[Auth_Success_Forwarder]**

5) Once you have those credentials you'll need to forward them to your own C++ backend in order to use the Sequence API, an example of this can be found in the **[Custom_Spectator_Pawn]**. This Pawn inherits from a C++ class **[Sqnc_Spec_Pawn]**, which implements a blueprint Callable function **[SetupCredentials(FCredentials_BE CredentialsIn)]**. This is callable within the child class **[Custom_Spectator_Pawn]**. Calling this function will forward the credentials to a C++ backend.

6) before we get into using the rest of the SequenceAPI we'll cover how to handle the Authentication side of things first.

### Custom UI Integration

In a C++ backend with a series of pass through **[UFUNCTIONS]** setup similarly to **[SequenceBackendManager.h/.cpp]**. Each of these calls are implemented in **[UAuthenticator]** you just need to pass through the data with YOUR UAuthenticator UObject

```clike
//This call is platform dependent on windows & mac this is required for SSO WIP
UFUNCTION(BlueprintCallable, CATEGORY = "Login")
FString GetLoginURL(const ESocialSigninType& Type); 

//This Call is made after you've collected the ID_Token (Mac & Windows only) WIP
UFUNCTION(BlueprintCallable, CATEGORY = "Login")
void SocialLogin(const FString& IDTokenIn);

//This Call is made after you've collected the email address from the Users in the UI
UFUNCTION(BlueprintCallable, CATEGORY = "Login")
void EmailLogin(const FString& EmailIn);

//This is call is made after the Delegate **[AuthRequiresCode]** is fired
UFUNCTION(BlueprintCallable, CATEGORY = "Login")
void EmailCode(const FString& CodeIn);

//Optional call used to check if the credentials on disk are valid or not//
UFUNCTION(BlueprintCallable, Category = "Login")
bool StoredCredentialsValid();
```


To start you'll want to create a **[UAuthenticator]** UObject like so **[UAuthenticator + Auth = NewObject<UAuthenticator>()]**, this UObject manages the authentication side of Sequence.

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

Where **[CallShowAuthSuccessScreen]** is defined in `SequenceBackendManager.h` like so:

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

### Email based Authentication

1) To start email based authentication you'll start it with this call **[EmailLogin(const FString& EmailIn)]**, supplying an email you've collected from the User in your GUI.

2) Next **[AuthRequiresCode]** will fire when the backend is ready to receive the Code from your UI. Collect this code from your GUI and send it to the authenticator using **[EmailCode(CodeIn)]**.

3) Finally **[AuthSuccess]** will fire with a Credentials_BE struct as a parameter. This is your non registered credentials from EmailAuth. You are done Email Based Auth.

### Social Signin based Authentication on Desktop

1) To start SSO based authentication with desktop you can either use your own implementation to get the necessary id_token or you can make use of Unreal's web browser plugin.

2) With whatever implementation you chose you can forward the colleted id_token to the UAuthenticator object with **[SocialLogin(const FString& IDTokenIn)]**, after which **[AuthSuccess]** will fire and you're done desktop based SSO.

### Social Signin based Authentication on Mobile

WIP

### Sequence API

After you've completed initial authentication and have intercepted the credentials either through your UI or ours, to use the Sequence API you'll need to create a **[USequenceWallet]** by using:

```clike
**[USequenceWallet * Api = USequenceWallet(CredentialsIn)]** or **[USequenceWallet * Api = USequenceWallet::Make(CredentialsIn, ProviderURL)]**
```

Once you have your **[USequenceWallet]** UObject call **[Api>RegisterSession(OnSuccess,GenericFailure)]** this will register your credentials with the Sequence API. Note not calling Register prior to make any others calls will result in errors as a UserWallet hasn't been supplied until this point.

***

Assuming you've setup your controlling Actor with the **[Sequence_Pawn_Component_BP]**
The sequence pawn component has functions to do the following:

Setup Sequence (sets up the sequence based systems), requires playerController input

Show GUI Shows the UI

Hide GUI Hides the UI

GUI Visible Simple Visibility test for the UI

Switch Platform (Switches which mode the UI will be in and how it will be displayed)

Note: this doesn't rotate the application into any one view it just make the UI responsive to that type of view.

Modes:
- Desktop (default)
- Mobile Portrait (Custom built for portrait mode reducing the X width where ever possible)
- Mobile Landscape

***

### Sequence Unreal API

The Sequence Unreal plugin is broken up into a few different parts in order to manage all of the differing functionality. Here we'll go through each parts purpose then summarize how to use them all together below

The SDK automatically stores credentials on disk in the following cases:
1) On Successful Auth
2) On Successful Registering of a session
3) On Successful Closing of a session


#### USequenceWallet

To get a `USequenceWallet` call either:

```
USequenceWallet::Make(FCredentials_BE CredentialsIn)
USequenceWallet::Make(FCredentials_BE CredentialsIn, FString ProviderURL)
```

Where the Credentials you give are the credentials you received from the UAuthenticator when **[AuthSuccess]** fires, or you can use the call Auth->**[GetStoredCredentials]**(), where Auth is of the type **[UAuthenticator]**. If you are using StoredCredentials please ensure they are valid by checking the wrapping Structs FStoredCredentials_BE.GetValid() flag returned from **[GetStoredCredentials]**, the providerURL is the url of the provider you wish to use.

Once you have your **[USequenceWallet]** UObject please ensure that you've registered the session using **[RegisterSession]** before attempting to make other calls to the API.


#### Sequence API Methods

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

One thing to be aware of is keep an eye on capturables if you have lots of nested TFunctions it's very easy to miss something and start over writing memory. If you require lots of nesting swapping to a better approach using UFUNCTION callbacks helps to avoid these problems similar to how things are done in **[UAuthenticator.h/cpp]**


### Packaging

To set your system up for Packaging please refer to the following links:

- [Windows and macOS](https://docs.unrealengine.com/5.0/en-US/packaging-unreal-engine-projects/)
- [Android](https://docs.unrealengine.com/4.27/en-US/SharingAndReleasing/Mobile/Android/PackagingAndroidProject/)
- [iOS](https://docs.unrealengine.com/5.0/en-US/packaging-ios-projects-in-unreal-engine/)

#### Google SSO Setup
In order to be able to properly use Google Auth, create and place the Keystore file by following [these instructions](https://docs.unrealengine.com/5.1/en-US/signing-android-projects-for-release-on-the-google-play-store-with-unreal-engine/).

You will also need to generate an **[Android client ID]** and a **[Web Application client ID]** for your application, as well as place the **[Web Application client ID]** in the `Config/Config.h` `FAuthenticatorConfig.GoogleClientID` field.

Refer to [these docs](https://developers.google.com/identity/one-tap/android/get-started#api-console) to generate **[Android client ID]** and **[Web Application client ID]**.

[This guide](https://developers.google.com/android/guides/client-auth) helps explain how to collect SHA-1 key fingerprints for the **[Android client ID]**.

#### iOS
For iOS apps you also need to setup provisioning, [following these docs](https://docs.unrealengine.com/5.1/en-US/setting-up-ios-tvos-and-ipados-provisioning-profiles-and-signing-certificates-for-unreal-engine-projects/).

#### Unreal and Xcode Specifics
During the Unreal Package process there is a very high chance that it will fail due to a code signing error, in the event that this happens please take the following steps within XCode to get your packaged .app file

1) After packaging the project in Unreal, open the Xcode project (Sequence-unreal folder -> Intermediate -> ProjectFilesIOS -> SequenceUnreal.xcodeproj)
2) Click on the project name on the left hand side to open up project settings
3) Click the Build Phase Tab
4) Click on the ‘+’ icon at the top left
5) Select Run Script
6) Drag the new run script to one below from the last item in the phase list
7) Expand the run script
8) In the script box, add the following command: `xattr -cr /path-to-your-project/sequence-unreal/Binaries/IOS/Payload/SequenceUnreal.app`
9) Click on the Build Settings tab
10) Click on each item under the Architectures header that contains macOS and hit the delete key
11) Click on the General tab
12) Click on Mac and Applevision Pro under supported destinations and hit the delete key
13) Now the project can be built (if the build fails at first, wait a few moments then try again. It can sometimes take a bit before the build registers the run script)
14) Once you have finished running the project, and want to make changes to the code, REMEMBER to delete this xcodeproj file in the sequence-unreal folder to ensure that a new xcodeproj is creating when you packaging the project again.
