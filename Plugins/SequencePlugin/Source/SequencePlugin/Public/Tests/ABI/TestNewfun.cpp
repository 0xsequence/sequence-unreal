#include "Util/HexUtility.h"
#include "ABI/ABI.h"
#include "Misc/AutomationTest.h"
#include "Types/BinaryData.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestNewfun, "Public.Tests.ABI.TestNewfun",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestNewfun::RunTest(const FString& Parameters)
{
	auto addy = FAddress::From("71C7656EC7ab88b098defB751B7401B5f6d8976F");

	UE_LOG(LogTemp, Display, TEXT("The addy is %s"), *addy.ToHex());
	
	auto addy_arg = FABIArg{
		STATIC, FAddress::Size, addy.Arr
	};
	addy_arg.Log();
	FABIArg* Args = new FABIArg[2]{FABIArg::New(static_cast<uint32>(15)), addy_arg};

	auto BlockNumInt = Args[0].GetBlockNum() + Args[1].GetBlockNum();
	auto s = ABI::EncodeArgs("newFun(int,address)", Args, 2);
	UE_LOG(LogTemp, Display, TEXT("%s"), *s.ToHex());

	auto StringProp = FABIInt32Property(-1);
	auto AddressProp = FABIAddressProperty(addy);
	TArray<FABIProperty*> Properties;
	Properties.Push(&StringProp);
	Properties.Push(&AddressProp);
	s = ABI::Encode("newFun(int,address)", Properties);
	UE_LOG(LogTemp, Display, TEXT("%s"), *s.ToHex());

	if(false)
	{
		//UE_LOG(LogTemp, Display, TEXT("HEADER: %s"), *HashToHexString(GMethodIdByteLength, &s.Arr[0]));
		UE_LOG(LogTemp, Display, TEXT("HEADER: %s"), *BytesToHex(&s.Arr[0], GMethodIdByteLength));
		for(auto i = 0; i < BlockNumInt; i++)
		{
			auto Addr = GMethodIdByteLength + GBlockByteLength * i;
			//UE_LOG(LogTemp, Display, TEXT("%i %s"), Addr, *HashToHexString(GBlockByteLength, &s.Arr[Addr]));
			UE_LOG(LogTemp, Display, TEXT("%i %s"), Addr, *BytesToHex(&s.Arr[Addr], GBlockByteLength));
		}
	}
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
