#include "TestSequenceAPI.h"

#include "ABI/ABI.h"
#include "ABI/ABITypes.h"
#include "Sequence/SequenceAPI.h"
#include "Authenticator.h"

void SequenceAPITest::RunTest(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	SequenceAPI::FSequenceWallet Api = SequenceAPI::FSequenceWallet(Auth->GetStoredCredentials().GetCredentials());
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("Test Failed", Error);
	};

	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API Tests]========================"));

	Api.RegisterSession(OnSuccess,GenericFailure);
	//Api.ListSessions(OnSuccess,GenericFailure);
	//Api.SignMessage("Some Epic Message",OnSuccess,GenericFailure);
	//Api.SessionValidation(OnSuccess,GenericFailure);
	//Api.CloseSession(OnSuccess,GenericFailure);
}
