#include "tests/ContractTest.h"

#include "Types/BinaryData.h"
#include "EthTransaction.h"
#include "HexUtility.h"
#include "Provider.h"
#include "Types/ContractCall.h"
#include "ABI/ABI.h"
#include "ABI/ABITypes.h"

void ContractTest::NonPayable(FAddress ContractAddress4, FString Url, FAddress Address2, FPrivateKey PrivateKey2, TFunction<void (FString)> OnSuccess, TFunction<void (FString, SequenceError)> OnFailure)
{
	const TFailureCallback GenericFailure = [=](const SequenceError Error)
	{
		OnFailure("", Error);
	};
	
	//nonpayable method
	auto RefillAmount = FABIUInt32Property(5);
	TArray<FABIProperty*> Properties2{&RefillAmount};
	const FNonUniformData ContractData = ABI::Encode("refill(uint256)", Properties2);

	Provider(Url).TransactionCount(Address2, EBlockTag::Latest, [=](uint64 Nonce)
	{
		Provider(Url).GetGasPrice([=](FNonUniformData GasPrice)
		{
			const auto ContractTo = ContractAddress4;

			const auto ContractCall2 = FContractCall{
			TOptional<FAddress>(Address2),
			ContractAddress4,
			TOptional<uint64>(),
			TOptional<uint64>(),
			TOptional<uint64>(),
			TOptional<FString>(ContractData.ToHex())};

			Provider(Url).EstimateContractCallGas(ContractCall2, [=](FNonUniformData ContractCallGas)
			{
				//call purchase function in smart contract
				const auto ContractTransaction = FEthTransaction{
					FBlockNonce::From(IntToHexString(Nonce)),
					GasPrice,
					ContractCallGas,
					ContractTo,
					HexStringToBinary(""),
					ContractData,
				};

				Provider(Url).NonViewCall(ContractTransaction, PrivateKey2, 1337, [=](FNonUniformData MethodHash)
				{
					GetBalance(ContractAddress4, [=](const uint64 Value)
					{
						if(Value != 102)
						{
							GenericFailure(SequenceError{TestFail, "LargeTest: Contract4 Balance is incorrect (not 102)"});
						}
						else
						{
							OnSuccess("LargeTest: NonPayable Call Successful");
						}
					}, GenericFailure);
				}, GenericFailure);
			}, GenericFailure);
		}, GenericFailure);
		
	}, GenericFailure);
}

void ContractTest::GetBalance(const FAddress To, TSuccessCallback<uint64> OnSuccess, TFailureCallback OnFailure)
{
	FString Url = "http://localhost:8545/";
	Provider Provider(Url);
	TArray<FABIProperty*> mArgs = TArray<FABIProperty*>();
	FABIAddressProperty myAddressProperty = FABIAddressProperty(To);
	mArgs.Add(&myAddressProperty);
	const FNonUniformData EncodedData = ABI::Encode("cupcakeBalances(address)", mArgs);
	const FContractCall Call = FContractCall{
		TOptional<FAddress>(),
		To,
		TOptional<uint64>(),
		TOptional<uint64>(),
		TOptional<uint64>(),
		TOptional<FString>(EncodedData.ToHex()),
	};
	Provider.Call(Call, EBlockTag::Latest, [=](FNonUniformData ContractBalance)
	{
		TOptional<uint64> Value = HexStringToUint64(ContractBalance.ToHex());

		if(!Value.IsSet())
		{
			OnFailure(SequenceError{ResponseParseError, "Contract Balance failed to parse into integer"});
		}
		else
		{
			OnSuccess(Value.GetValue());
		}
	}, OnFailure);
}

void ContractTest::CallMethods(const FAddress ContractAddress4, FString Url, FAddress Address2, FPrivateKey PrivateKey2, TFunction<void (FString)> OnSuccess, TFunction<void (FString, SequenceError)> OnFailure)
{
	const TFailureCallback GenericFailure = [=](SequenceError Error)
	{
		OnFailure("", Error);
	};
	
	//call method with the contract deployed above

	//view method
	GetBalance(ContractAddress4, [=](uint64 Balance)
	{
		if(Balance != 100)
		{
			GenericFailure(SequenceError{TestFail, "LargeTest: Contract4 balance is incorrect (not 100)"});
		}

		//payable method
		FABIUInt32Property PurchaseAmount = FABIUInt32Property(3);
		TArray<FABIProperty*> Properties{&PurchaseAmount};
		const FNonUniformData ContractData = ABI::Encode("purchase(uint256)", Properties);
		
		Provider(Url).TransactionCount(Address2, EBlockTag::Latest, [=](uint64 Nonce)
		{
			Provider(Url).GetGasPrice([=](const FNonUniformData Price)
			{
				const FAddress ContractTo = ContractAddress4;
				const uint64 ContractValue = 3000000000000000000;

				const FContractCall MContractCall = FContractCall
				{
					TOptional<FAddress>(),
					ContractAddress4,
					TOptional<uint64>(),
					TOptional<uint64>(),
					TOptional<uint64>(ContractValue),
					TOptional<FString>(ContractData.ToHex()),
				};
				
				
				Provider(Url).EstimateContractCallGas(MContractCall, [=](FNonUniformData CallGas)
				{
					//call purchase function in smart contract
					FEthTransaction contractTransaction = FEthTransaction
					{
						FBlockNonce::From(IntToHexString(Nonce)),
						Price,
						CallGas,
						ContractTo,
						HexStringToBinary(IntToHexString(ContractValue)),
						ContractData,
					};

					Provider(Url).NonViewCall(contractTransaction, PrivateKey2, 1337, [=](FNonUniformData Data)
					{
						//view method
						GetBalance(ContractAddress4, [=](const uint64 MyBalance)
						{
							if(MyBalance != 97)
							{
								GenericFailure(SequenceError{TestFail, "LargeTest: Contract4 balance is incorrect (not 97)"});
							}
							else
							{
								OnSuccess("LargeTest: Method call successful!");
								NonPayable(ContractAddress4, Url, Address2, PrivateKey2, OnSuccess, OnFailure);
							}
						}, GenericFailure);
					}, GenericFailure);
					
					
				}, GenericFailure);
			}, GenericFailure);
		}, GenericFailure);
	}, GenericFailure);
}


void ContractTest::DeployContract4(const FString Url, const FString VMByteCode, const TFailureCallback GenericFailure, const TFunction<void (FString)> OnSuccess, TFunction<void (FString, SequenceError)> OnFailure, FPrivateKey PrivateKey2, FAddress Address2)
{
	//deploy PrivateKey2
	Provider(Url).DeployContractWithHash(VMByteCode, PrivateKey2, 1337, [=](FAddress ContractAddress4, FNonUniformData Hash)
	{
		Provider(Url).TransactionReceipt(FHash256::From(Hash.Arr), [=](FTransactionReceipt Receipt)
		{	
			if(Receipt.ContractAddress.ToHex() != ContractAddress4.ToHex()) return OnFailure("", SequenceError{TestFail, "Receipt address 4 doesn't match contract address 4"});
			if(Receipt.From.ToHex() != Address2.ToHex()) return OnFailure("", SequenceError{TestFail, "Receipt field FROM doesn't match address 2"});
			if(Receipt.To.ToHex() != "0000000000000000000000000000000000000000") return OnFailure("", SequenceError{TestFail, "Receipt field TO is not the null address"});
		
			OnSuccess("LargeTest: Deployed contract 4/4");
			
			CallMethods(ContractAddress4, Url, Address2, PrivateKey2, OnSuccess, OnFailure);
		}, GenericFailure);
	}, GenericFailure);
}

void ContractTest::DeployContract3(const FString Url, const FString VMByteCode, const TFailureCallback GenericFailure, const TFunction<void (FString)> OnSuccess, TFunction<void (FString, SequenceError)> OnFailure, FPrivateKey PrivateKey1, FAddress Address1, FPrivateKey PrivateKey2, FAddress Address2)
{
	Provider(Url).DeployContractWithHash(VMByteCode, PrivateKey1, 1337, [=](const FAddress ContractAddress3, FNonUniformData Hash)
	{
		Provider(Url).TransactionReceipt(FHash256::From(Hash.Arr), [=](FTransactionReceipt Receipt)
		{	
			if(Receipt.ContractAddress.ToHex() != ContractAddress3.ToHex()) return OnFailure("", SequenceError{TestFail, "Receipt address 3 doesn't match contract address 3"});
			if(Receipt.From.ToHex() != Address1.ToHex()) return OnFailure("", SequenceError{TestFail, "Receipt field FROM doesn't match address 1"});
			if(Receipt.To.ToHex() != "0000000000000000000000000000000000000000") return OnFailure("", SequenceError{TestFail, "Receipt field TO is not the null address"});

			OnSuccess("LargeTest: Deployed contract 3/4");

			DeployContract4(Url, VMByteCode, GenericFailure, OnSuccess, OnFailure, PrivateKey2, Address2);
		}, GenericFailure);
	}, GenericFailure);
}

void ContractTest::DeployContract2(FString Url, FString LongByteCode, FString VMByteCode, TFailureCallback GenericFailure, TFunction<void (FString)> OnSuccess, TFunction<void (FString, SequenceError)> OnFailure, FPrivateKey PrivateKey1, FAddress Address1, FPrivateKey PrivateKey2, FAddress Address2)
{
	Provider(Url).DeployContractWithHash(LongByteCode, PrivateKey1, 1337, [=](FAddress ContractAddress2, FNonUniformData Hash)
	{
		UE_LOG(LogTemp, Display, TEXT("My deployed address is %s"), *ContractAddress2.ToHex());
		Provider(Url).TransactionReceipt(FHash256::From(Hash.Arr), [=](FTransactionReceipt Receipt)
		{	
			if(Receipt.ContractAddress.ToHex() != ContractAddress2.ToHex()) return OnFailure("", SequenceError{TestFail, "Receipt address 2 doesn't match contract address 2"});
			if(Receipt.From.ToHex() != Address1.ToHex()) return OnFailure("", SequenceError{TestFail, "Receipt field FROM doesn't match address 1"});
			if(Receipt.To.ToHex() != "0000000000000000000000000000000000000000") return OnFailure("", SequenceError{TestFail, "Receipt field TO is not the null address"});

			OnSuccess("LargeTest: Deployed contract 2/4");

			DeployContract3(Url, VMByteCode, GenericFailure, OnSuccess, OnFailure, PrivateKey1, Address1, PrivateKey2, Address2);
		}, GenericFailure);
	}, GenericFailure);
}

void ContractTest::DeployContracts(FString Url,  FString LongByteCode, FString SmallByteCode, FString VMByteCode, TFailureCallback GenericFailure, TFunction<void (FString)> OnSuccess, TFunction<void (FString, SequenceError)> OnFailure, FPrivateKey PrivateKey1, FAddress Address1, FPrivateKey PrivateKey2, FAddress Address2)
{
	Provider(Url).DeployContractWithHash(SmallByteCode, PrivateKey1, 1337, [=](FAddress Address, FNonUniformData Hash)
	{
		Provider(Url).TransactionReceipt(FHash256::From(Hash.Arr), [=](FTransactionReceipt Receipt)
		{
			if(Receipt.ContractAddress.ToHex() != Address.ToHex()) return OnFailure("", SequenceError{TestFail, "Receipt address 1 doesn't match contract address 1"});
			if(Receipt.From.ToHex() != Address1.ToHex()) return OnFailure("", SequenceError{TestFail, "Receipt field FROM doesn't match address 1"});
			if(Receipt.To.ToHex() != "0000000000000000000000000000000000000000") return OnFailure("", SequenceError{TestFail, "Receipt field TO is not the null address"});

			OnSuccess("LargeTest: Deployed contract 1/4");

			DeployContract2(Url, LongByteCode, VMByteCode, GenericFailure, OnSuccess, OnFailure, PrivateKey1, Address1, PrivateKey2, Address2);
		}, GenericFailure);
	}, GenericFailure);
}

