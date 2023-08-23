#include "TestSequenceAPI.h"

#include "ABI/ABI.h"
#include "ABI/ABITypes.h"
#include "Sequence/SequenceAPI.h"

void SequenceAPITest::RunTest(TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	SequenceAPI::FSequenceWallet Api = SequenceAPI::FSequenceWallet();
	const FFailureCallback GenericFailure = [OnFailure](const SequenceError Error)
	{
		OnFailure("", Error);
	};

	FUnsizedData Message = StringToUTF8("test message");
	auto tx = SequenceAPI::FTransaction{
		421613,
		FAddress::From("0x660250734f31644681ae32d05bd7e8e29fea29e1"),
		FAddress::From("0xc2f3169e84f69adbdba77b209292b8924b35c121"),
	};

	tx.Value = "10";

	Api.SendTransaction(tx, [=](FHash256 Hash)
	{
		UE_LOG(LogTemp, Display, TEXT("The Hash is %s"), *Hash.ToHex());
	}, GenericFailure);

	Api.GetWalletAddress([=](FAddress Address)
	{
		UE_LOG(LogTemp, Display, TEXT("The Address is %s"), *Address.ToHex())
	}, GenericFailure);
}
