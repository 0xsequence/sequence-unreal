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

	FUnsizedData Message = StringToUTF8("test message");
	auto tx = SequenceAPI::FTransaction_Sequence
	{
		421613,
		FAddress::From("660250734f31644681ae32d05bd7e8e29fea29e1"),
		FAddress::From("c2f3169e84f69adbdba77b209292b8924b35c121"),
	};

	tx.Value = "10";

	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API Tests]========================"));
	
	/*Api.SendTransaction(tx, [=](FHash256 Hash)
	{
		UE_LOG(LogTemp, Display, TEXT("[Success]: The Hash is %s"), *Hash.ToHex());
	}, GenericFailure);*/
	
	/*Api.GetWalletAddress([=](FAddress Address)
	{
		UE_LOG(LogTemp, Display, TEXT("[Success]: The Address is %s"), *Address.ToHex())
	}, GenericFailure);*/
}
