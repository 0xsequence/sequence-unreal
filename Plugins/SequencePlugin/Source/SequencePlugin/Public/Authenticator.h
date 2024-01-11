// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RequestHandler.h"
#include "Util/Structs/BE_Enums.h"
#include "Types/Wallet.h"
#include "Authenticator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthRequiresCode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthFailure);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthSuccess);

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UAuthenticator : public UObject
{
	GENERATED_BODY()
//Delegates
public:
	FOnAuthRequiresCode AuthRequiresCode;
	FOnAuthFailure AuthFailure;
	FOnAuthSuccess AuthSuccess;

private://Broadcast handlers
	void CallAuthRequiresCode();
	void CallAuthFailure();
	void CallAuthSuccess();
//vars
private:
	TEnumAsByte<ESocialSigninType> SocialSigninType;

	FString StateToken = TEXT("");
	FString Nonce = TEXT("");

	FString UrlScheme = TEXT("powered-by-sequence");
	FString RedirectURL = TEXT("https://3d41-142-115-54-118.ngrok-free.app/");

	FString GoogleAuthURL = TEXT("https://accounts.google.com/o/oauth2/auth");
	FString GoogleClientID = TEXT("970987756660-35a6tc48hvi8cev9cnknp0iugv9poa23.apps.googleusercontent.com");

	FString FacebookAuthURL = TEXT("https://www.facebook.com/v18.0/dialog/oauth");
	FString FacebookClientID = TEXT("");//TODO still need this

	FString DiscordAuthURL = TEXT("https://discord.com/api/oauth2/authorize");
	FString DiscordClientID = TEXT("");//TODO still need this

	FString AppleAuthURL = TEXT("https://appleid.apple.com/auth/authorize");
	FString AppleClientID = TEXT("");//TODO still need this

	FString Cached_IDToken;
	FString Cached_Email;
	//AWS
	FString IdentityPoolID = TEXT("us-east-2:42c9f39d-c935-4d5c-a845-5c8815c79ee3");
	FString UserPoolID = TEXT("FpPUBLAGt");
	FString Region = TEXT("us-east-2");
	FString AWSService = TEXT("kms");
	//FString Region = TEXT("us-east-1");//for testing!
	//FString AWSService = TEXT("s3");
	FString CognitoClientID = TEXT("5fl7dg7mvu534o9vfjbc6hj31p");
	FString KMSKeyID = TEXT("0fd8f803-9cb5-4de5-86e4-41963fb6043d");
	FString ProjectID = TEXT("nil");//TODO still need this we get this from builder?
	FString WaasVersion = TEXT("1.0.0");
	FString X_Sequence_Tenant = TEXT("9");
	FString WaasRPCURL = TEXT("https://d14tu8valot5m0.cloudfront.net/rpc");

	/*
	* Region: "us-east-2"
	* IdentityPoolId: "us-east-2:42c9f39d-c935-4d5c-a845-5c8815c79ee3"
	* kmsEncryptionKeyId: "arn:aws:kms:us-east-2:170768627592:key/0fd8f803-9cb5-4de5-86e4-41963fb6043d"
	* CognitoClientId: "5fl7dg7mvu534o9vfjbc6hj31p"
	* X-Sequence-Tenant: 9
	* WaaSVerion: "1.0.0"
	* WaaS RPC URL: https://d14tu8valot5m0.cloudfront.net/rpc
	*/

	/*
{"AuthenticationResult":
{"AccessToken":"eyJraWQiOiI1QW0xRkxacUNcL0lvNWEyNUhTbnBhTUVFTUJCSlwvazNZR2JwZDJnengxQUU9IiwiYWxnIjoiUlMyNTYifQ.eyJzdWIiOiIzZjI2ZWNmOS0wZDdkLTQ1OWItODNjZi02M2FlNjc1MWEzMmEiLCJpc3MiOiJodHRwczpcL1wvY29nbml0by1pZHAudXMtZWFzdC0yLmFtYXpvbmF3cy5jb21cL3VzLWVhc3QtMl9GcFBVQkxBR3QiLCJjbGllbnRfaWQiOiI1Zmw3ZGc3bXZ1NTM0bzl2ZmpiYzZoajMxcCIsIm9yaWdpbl9qdGkiOiI4ZDhhODQ2ZC1lMjJjLTRmOTYtOGM5ZC1iYTk4YzNjOTUyZGQiLCJldmVudF9pZCI6ImVmNTkwZDllLWQ1YjItNDIzNC04MTFkLTJiOGJmOGE4NjVlZiIsInRva2VuX3VzZSI6ImFjY2VzcyIsInNjb3BlIjoiYXdzLmNvZ25pdG8uc2lnbmluLnVzZXIuYWRtaW4iLCJhdXRoX3RpbWUiOjE3MDI2Njg3MTUsImV4cCI6MTcwMjY3MjMxNSwiaWF0IjoxNzAyNjY4NzE1LCJqdGkiOiJjOTEzYjgyYy1hMjBmLTRhZDMtOTMyOS0wOGQ0ZjI4MTkzOWEiLCJ1c2VybmFtZSI6ImNhbHZpbi5rb3JrQHplbWluZC5jYSJ9.IEaExQriNunNvgJ29INCWNFEhdymsUVenAxl6Y8u9UANRSUh8PV08kLzzjPG5N7B2uloBMSqgaUH_kM9A1CuBeRGC0uhmPaU2J7cuyxVG8ilo66VizfZv2SedFqnvbAav0M_IkavKHQqxrvGdgCD5iLq3ZgjZXkTwF8rjw7L_fSUzGk32DRorc9oKbxcHdF4qd_lJdS1TEjv0quMS5K5eClU4WSvD09kacIteFHdDiqqCqIri0FJy9Tae3k_wTLEN3VOcYVqz8x3cc3PCeZPYK3unAU3AmP4gpnBuTNm2Q6O2OdJrq11xDu_nP9phOUbOPsCRlEwViP99dh9F0BRRw",
"ExpiresIn":3600,
"IdToken":"eyJraWQiOiJ2MkJJZTIrd2lDUzhOMjhFYzBKdTd5bmVHeUNUOUZHRVppRGxIdmNZckU0PSIsImFsZyI6IlJTMjU2In0.eyJzdWIiOiIzZjI2ZWNmOS0wZDdkLTQ1OWItODNjZi02M2FlNjc1MWEzMmEiLCJlbWFpbF92ZXJpZmllZCI6dHJ1ZSwiaXNzIjoiaHR0cHM6XC9cL2NvZ25pdG8taWRwLnVzLWVhc3QtMi5hbWF6b25hd3MuY29tXC91cy1lYXN0LTJfRnBQVUJMQUd0IiwiY29nbml0bzp1c2VybmFtZSI6ImNhbHZpbi5rb3JrQHplbWluZC5jYSIsIm9yaWdpbl9qdGkiOiI4ZDhhODQ2ZC1lMjJjLTRmOTYtOGM5ZC1iYTk4YzNjOTUyZGQiLCJhdWQiOiI1Zmw3ZGc3bXZ1NTM0bzl2ZmpiYzZoajMxcCIsImV2ZW50X2lkIjoiZWY1OTBkOWUtZDViMi00MjM0LTgxMWQtMmI4YmY4YTg2NWVmIiwidG9rZW5fdXNlIjoiaWQiLCJhdXRoX3RpbWUiOjE3MDI2Njg3MTUsImV4cCI6MTcwMjY3MjMxNSwiaWF0IjoxNzAyNjY4NzE1LCJqdGkiOiJjMzIyZTlkZS02MzgyLTRlNjctOWUyOS0yNTQ2MmQ2OWIyMGQiLCJlbWFpbCI6ImNhbHZpbi5rb3JrQHplbWluZC5jYSJ9.AflQVGAOKTo-xWGR8Uf24pPxcQ9njxVr9syxD3RaCQw9T7J0rpKyrRs0m4tMjmBqSXssfGsB7Ld_wFyV_McgT2MEX4Qs6YzhqrEeMZhgFeOtZHXZTD_sb3dv_wZ3qUrFigZUbvnVloAAhVULT-TI_9J5kHbGtYkCHb-ZdO4d6FSHrllcoP_IWzGDo6gHC_8mvLAEkAwActRLz9wlQGZyKoKYrZdHbaFyS-hMqKIyT_mRwIDq-fwYzz-qGMllCE59XIHBJZBtQW_lw9Mwz5uqR3VpXLJW6T1gs3WqZ-g18LM2hI3mXFhUuSM_DrhTMGAqVzjevf8itGsMXzARTaB1Hw",
"RefreshToken":"eyJjdHkiOiJKV1QiLCJlbmMiOiJBMjU2R0NNIiwiYWxnIjoiUlNBLU9BRVAifQ.MGk_CMVNyw90_f8OGwQ5xIjP-CJmbuEDc1omePXGeiZwZxRnAC7yoB8EpXjFJtDQNIdI-Osmv7h8GxMf9GpCpygkQYDAgntl587lWlIFMdsWzA4AodtaZhkP4thDm5-DVLVbvUBsCGQghb8lGuJYT3XptL4cF2OAPWN0wD9f6fiUZ9jiSXNogkjYPSHjZWJxo2cDMf4XqPuFu0ytj34ZerH0Mxsic2Baf52zRTMS43EG74F-uEjC3CZH59bwLmd0aVPDXhlnkqSbk1Tw8oQVhkyGpY2ZBvE5ybQHTdLPwcbIxuPAMWvBZWqtmvp4d4b56psaS5AADrCaeZgo8vaCnQ.q4NbK8PkzRXxeVRx.vDQxr9D0lZmKCsnZz_zVXh5HYiXWYbI2QSjKyiy4QlKOh8E5Ingo9fZWrZy4qgNcBW0CAUoiyJslZlI53EZEF6iL3ACeOtKejmRbScrUZsRxXH0ISRKjC9jYE6HPJ15vsBmJ2R34OBpZXIDByu6fc-PzYI8AdztkfgHvQh0bAZcfHFVklNP4Q7_mEa2EC3xUE0uqAMhKih0CslmP6nvdPFguW1GmTziVeDDrZcEQ2BWZEJnA9lBPcjdW5wc5eC_eJdWKBc3FCAi0CyuCmUmA-X2ugubE2y5RjdUIm0VGiJ6Zz15RCIbIba5UrQSsDo72xETJj075vPJA04rCEvovm915itVLEwNx2C3mUKq36qGOy8K94k008k5R615QxTC3EI6O3bIi6Mg5iQM8qO1w-s5Qf1y_067C4ZA6TDwWHEuwg-1Ig6V1rHmGQipXdDWrUdOLKWlRjM6S05DeGn3rK9kFeG8-peAzWDw1ZyGZ2Bx3gwvr_Ln7C5Qv4DejEwf8HLznSlNXAxsSmcVsbvAOyRohJ5aXATL8767EMmDFl3sDagZS1zRIrDKK1oo3zW6tS25udL6_PChIW44taqtYyaOFqGBECJ6uzeDQhXFZnTvIfAc-R6DuL4f0EBO8W-KCIMhrnjJaFXiL9dTxVQLGaAx_FfasBl2Y4mRYTsoGG5UF2Oa6TZVbGtT-mZ5fH-wOTWqckC51TdVvqkNlgNmSicjfTxcvsAQfA57-KxL3aQx902Cj5OYToxJxQQ6W6-lxUUpk9VWKY8eczk8S9VRVoV1jkiT2gYhnJRtUWzLS69H8EYyGzx-nTvvC298g58JObNTi8BNizz3jXIX1YDor7mNKy8spD8c0oxe6CnnXq1h58EaTW3XiolJRYvzf2Tdn0MeubxzjdUl2i199vMpL55oxEMv9TeRzp2hnLWC9GvppTpRXvYfMHWhqSdIxPApQqE8NQWqZ4PORRoPj7ZglqZYLXbkwN6yTZUyrAxxitEuQppu7YT_qvlpq7nngF0ICSd45ho6tIR65FljGgUpFLKiOrKCG2XNHZLhX5xgD9XRM-lbQAm4a4gLLYwzD7kIoXazzdwbCqVQ8rJY2qSABVc4K57-0X5-xA4NSxhIigR1KdwIngzOCFEZivXNt2Dp5pXtLA33_AqGYT0wPtjhkHlbHZkPEg03-umwuU-1t-0SI0XJCDtV0j_opXnF3u51gc4x2k4m5j4orFaQ5drKURJl6nQX5JXYlP-6DSAwj6JRZpJZTbgIijc0nH8TZInZ0L_EmlXQwYuMOV9WAsljn-tcJzawbZWk.FIz281gkSFESBDYahkDnsw",
"TokenType":"Bearer"},
"ChallengeParameters":{}}
	*/

	// //us-east-2:170768627592
	//cognito-idp.us-east-1.amazonaws.com/us-east-1_123456789
	//Amazon Cognito user pool: cognito-idp.<region>.amazonaws.com/<YOUR_USER_POOL_ID>, for example, cognito-idp.us-east-1.amazonaws.com/us-east-1_123456789 
	//"cognito-idp.us-east-2.amazonaws.com/170768627592"
	const FString AWSIssuer = "cognito-idp."+this->Region+".amazonaws.com/"+this->Region+"_"+this->UserPoolID;
	TMap<ESocialSigninType, FString> ProviderMap = { {ESocialSigninType::Google,"accounts.google.com"},{ESocialSigninType::FaceBook,"graph.facebook.com"},{ESocialSigninType::AWS,AWSIssuer} };
	TArray<FString> PWCharList = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","0","1","2","3","4","5","6","7","8","9"};

	const int32 EmailAuthMaxRetries = 2;
	int32 EmailAuthCurrRetries = EmailAuthMaxRetries;

	//From GetId
	FString IdentityId = TEXT("nil");

	//From GetCredentialsForIdentity
	FString AccessKeyId = TEXT("AKIAIOSFODNN7EXAMPLE");
	FString SecretKey = TEXT("wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY");
	//FString SecretKey = TEXT("wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY");
	FString SessionToken = TEXT("nil");

	//From KMSGenerateDataKey
	FString PlainText = TEXT("nil");
	FString CipherTextBlob = TEXT("nil");//this is the transport key and needs to be secured

	//From InitiateAuth
	FString ChallengeSession = TEXT("nil");

	//Gonna need to free this in a destructor unless I swap it to a UObject to use garbage collection
	FWallet * SessionWallet;
private:
	UAuthenticator();
public:
	FString GetSigninURL();

	FString GetRedirectURL();

	void SetSocialLoginType(ESocialSigninType Type);

	void SocialLogin(const FString& IDTokenIn);

	void EmailLogin(const FString& EmailIn);

	void EmailLoginCode(const FString& CodeIn);
private:
	bool CanRetryEmailLogin();

	void ResetRetryEmailLogin();

	FString GenerateSigninURL(FString AuthURL, FString ClientID);

	FString BuildAWSURL(const FString& Service);

	FString GenerateSignUpPassword();

	FString BuildYYYYMMDD(const FDateTime& Date);

	FString BuildFullDateTime(const FDateTime& Date);

	FString BuildScope(const FDateTime& Date);

	FString BuildCanonicalRequest(const FString& URI, const FDateTime& Date, const FString& Payload);//step 1

	FString BuildStringToSign(const FDateTime& Date, const FString& CanonicalRequest);//step 2

	TArray<uint8_t> BuildSigningKey(const FDateTime& Date);//step 3

	FString BuildSignature(const TArray<uint8_t>& SigningKey, const FString& StringToSign);//step 4

public:	FString BuildKMSAuthorizationHeader(const FDateTime& Date, const FString& URI, const FString& Payload);

	//RPC Calls//

	void CognitoIdentityGetID(const FString& PoolID,const FString& Issuer, const FString& IDToken);

	void CognitoIdentityGetCredentialsForIdentity(const FString& IdentityID, const FString& IDToken, const FString& Issuer);

	void KMSGenerateDataKey();

	void CognitoIdentityInitiateAuth(const FString& Email);

	void CognitoIdentitySignUp(const FString& Email, const FString& Password);

	void AdminRespondToAuthChallenge(const FString& Email, const FString& Answer, const FString& ChallengeSessionString);

	//Sequence Specific//

	void AuthWithSequence(const FString& IDTokenIn, const TArray<uint8_t>& Key);

	//Sequence Specific//

	//RPC Calls//

	TSharedPtr<FJsonObject> ResponseToJson(const FString& response);

	void AuthorizedRPC(const FString& Authorization, const FDateTime& Date, const FString& Url, const FString& AMZTarget, const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

	void RPC(const FString& Url,const FString& AMZTarget,const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);
};