#include "TestSequenceAPI.h"

#include "Sequence/SequenceAPI.h"
#include "Authenticator.h"

void SequenceAPITest::RunTest(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API Tests]========================"));

	//Api->RegisterSession(OnSuccess,GenericFailure);
	//Api->ListSessions(OnSuccess,GenericFailure);
	Api->SignMessage("hi",OnSuccess,GenericFailure);
	//Api->CloseSession(OnSuccess,GenericFailure);
}

//quinn
//{"sessionId": "0x9372D71FFc4490c1058C4bDfa2Fcda26139c782B","intentJson": "{\"version\":\"1.0.0\",\"packet\":{\"code\":\"signMessage\",\"expires\":1706122882,\"issued\":1706122852,\"message\":\"0x19457468657265756D205369676E6564204D6573736167653A0A36206A6B206B6A\",\"network\":\"137\",\"wallet\":\"0x2D566542570771c264b98959B037f4eb7534caaA\"},\"signatures\":[{\"session\":\"0x9372D71FFc4490c1058C4bDfa2Fcda26139c782B\",\"signature\":\"0x208d56a500512884dbe41658e692abe5bd7b2f3da070e269a4aa35f303b9d4231081b347ee0c2b83d0729616c93c8ef203007f7cb7abc4bf807b286c2f1ae4111c\"}]}"}
//                                                                                                                                                                                               0x19457468657265756d205369676e6564204d6573736167653a0a326869                         
//{"sessionId":"0x3d95F190C7432D7e932DA1E2050beD6F40B19F6c","intentJson":"{\"version\":\"1.0.0\",\"packet\":{\"code\":\"signMessage\",\"expires\":1706300395,\"issued\":1706300365,\"message\":\"0x19457468657265756D205369676E6564204D6573736167653A0A326869\",\"network\":\"137\",\"wallet\":\"0x2D566542570771c264b98959B037f4eb7534caaA\"},\"signatures\":[{\"session\":\"0x3d95F190C7432D7e932DA1E2050beD6F40B19F6c\",\"signature\":\"0x7979751f2914b4e0a051b570af1369435d7dc63b8b18d76dce0e7a88e1d07c3d381aa64f1099ea508dad3f9a32a3cf858653fb442c3b8b31df1a51c70fafbb971c\"}]}"}
//{"sessionId":"0x7C0e0ACB73Fe6C3088A130804857C744A52bA29F","intentJson":"{\"version\":\"1.0.0\",\"packet\":{\"code\":\"signMessage\",\"expires\":1706633528,\"issued\":1706547128,\"message\":\"0x3BytesOutonKeccakHashesHere0000000000000000000000000000000\",\"network\":\"137\",\"wallet\":\"0xDdF51fe5a7D618144E117fD23Ac21A487e397C91\"},\"signatures\":[{\"session\":\"0x7C0e0ACB73Fe6C3088A130804857C744A52bA29F\",\"signature\":\"0x300a50b010d8bdcc8ce7e1f6e0656635c319d0dbb94458d7e31c34456d0c3a750b230abc0529851e91343571efbad73fcc3cb32ae063b40be2644fe7d5a2eeba00\"}]}"}