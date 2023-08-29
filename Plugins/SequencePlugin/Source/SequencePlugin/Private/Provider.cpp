// Fill out your copyright notice in the Description page of Project Settings.


#include "Provider.h"

#include "Eth/Crypto.h"
#include "Eth/EthTransaction.h"
#include "Types/BinaryData.h"
#include "Util/HexUtility.h"
#include "HttpManager.h"
#include "Util/JsonBuilder.h"
#include "JsonObjectConverter.h"
#include "RequestHandler.h"
#include "Types/ContractCall.h"
#include "Types/Header.h"

FString TagToString(EBlockTag Tag)
{
	switch (Tag)
	{
	case Latest:
		return "latest";
	case Earliest:
		return "earliest";
	case Pending:
		return "pending";
	case Safe:
		return "safe";
	case Finalized:
		return "finalized";
	}

	return "";
}

void Provider::BlockByNumberHelper(FString Number, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
{
	const FString Content = FJsonBuilder().ToPtr()
		->AddString("jsonrpc", "2.0")
		->AddInt("id", 1)
		->AddString("method", "eth_getBlockByNumber")
		->AddArray("params").ToPtr()
			->AddValue(Number)
			->AddBool(true)
			->EndArray()
		->ToString();

	const FString MyUrl = this->Url;
	SendRPCAndExtract<TSharedPtr<FJsonObject>>(
		Url,
		Content,
		OnSuccess,
		[MyUrl](FString Json)
		{
			TResult<TSharedPtr<FJsonObject>> Obj = Provider(MyUrl).ExtractJsonObjectResult(Json);

			

			if(Obj.HasValue() && Obj.GetValue() != nullptr)
			{
				return Obj;
			}
			
			if(Obj.HasValue() && Obj.GetValue() == nullptr)
			{
				TResult<TSharedPtr<FJsonObject>> Val = MakeError(FSequenceError(EmptyResponse, "Json response is null"));
				return Val;
			}

			return Obj;
		},
		OnFailure
	);
}

Provider Provider::Copy()
{
	return Provider(Url);
}




Provider::Provider(FString Url) : Url(Url)
{
}

void  Provider::BlockByNumber(uint64 Number, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
{
	BlockByNumberHelper(ConvertString(IntToHexString(Number)), OnSuccess, OnFailure);
}

void Provider::BlockByNumber(EBlockTag Tag, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
{
	BlockByNumberHelper(ConvertString(TagToString(Tag)), OnSuccess, OnFailure);
}

void Provider::BlockByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
{
	const FString Content = RPCBuilder("eth_getBlockByHash").ToPtr()
        ->AddArray("params").ToPtr()
	        ->AddString(Hash.ToHex())
	        ->AddBool(true)
	        ->EndArray()
        ->ToString();

	const FString MyUrl = this->Url;

	SendRPCAndExtract<TSharedPtr<FJsonObject>>(Url, Content,
		OnSuccess,
		[MyUrl](FString Result)
		{
			return Provider(MyUrl).ExtractJsonObjectResult(Result);
		},
		OnFailure);
}

void Provider::BlockNumber(TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure)
{
	const FString Content = RPCBuilder("eth_blockNumber").ToString();
	const FString MyUrl = this->Url;
	SendRPCAndExtract<uint64>(Url, Content,
		OnSuccess,
		[MyUrl](FString Result)
		{
			return Provider(MyUrl).ExtractUIntResult(Result);
		},
		OnFailure);
}

void Provider::HeaderByNumberHelper(FString Number, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure)
{
	BlockByNumberHelper(Number, [OnSuccess](TSharedPtr<FJsonObject> Json)
	{
		OnSuccess(JsonToHeader(Json));
	}, OnFailure);
}

void Provider::NonceAtHelper(FString Number, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure)
{

	TSuccessCallback<TSharedPtr<FJsonObject>> BlockCallback = [OnSuccess](TSharedPtr<FJsonObject> Json)
	{
		FString Hex = Json->GetStringField("nonce");
		FBlockNonce Nonce = FBlockNonce::From(HexStringToHash(FBlockNonce::Size, Hex));
		OnSuccess(Nonce);
	};
	
	this->BlockByNumberHelper(Number, BlockCallback, OnFailure);
}

void Provider::HeaderByNumber(uint64 Id, TFunction<void (FHeader)> OnSuccess, FFailureCallback OnFailure)
{
	HeaderByNumberHelper(ConvertString(IntToHexString(Id)), OnSuccess, OnFailure);
}

void Provider::HeaderByNumber(EBlockTag Tag, TFunction<void (FHeader)> OnSuccess, FFailureCallback OnFailure)
{
	HeaderByNumberHelper(ConvertString(TagToString(Tag)), OnSuccess, OnFailure);
}

void Provider::HeaderByHash(FHash256 Hash, TFunction<void (FHeader)> OnSuccess, FFailureCallback OnFailure)
{
	BlockByHash(Hash, [OnSuccess](TSharedPtr<FJsonObject> Json)
	{
		const FHeader Header = JsonToHeader(Json);
		OnSuccess(Header);
	}, OnFailure); 
}

void Provider::TransactionByHash(FHash256 Hash, TFunction<void (TSharedPtr<FJsonObject>)> OnSuccess, FFailureCallback OnFailure)
{
	const FString Content = RPCBuilder("eth_getTransactionByHash").ToPtr()
		->AddArray("params").ToPtr()
			->AddString(Hash.ToHex())
			->EndArray()
		->ToString();

	const FString MyUrl = this->Url;
	SendRPCAndExtract<TSharedPtr<FJsonObject>>(Url, Content,
		OnSuccess,
		[MyUrl](FString Result)
		{
			return Provider(MyUrl).ExtractJsonObjectResult(Result);
		},
		OnFailure);
}

void Provider::TransactionCount(FAddress Addr, uint64 Number, TFunction<void (uint64)> OnSuccess, FFailureCallback OnFailure)
{
	const FString Content = RPCBuilder("eth_getTransactionCount").ToPtr()
		->AddArray("params").ToPtr()
			->AddString("0x" + Addr.ToHex())
			->AddValue(ConvertString(IntToHexString(Number)))
			->EndArray()
		->ToString();

	const FString MyUrl = this->Url;
	SendRPCAndExtract<uint64>(Url, Content,
		OnSuccess,
		[MyUrl](FString Result)
		{
			return Provider(MyUrl).ExtractUIntResult(Result);
		},
		OnFailure);
}

void Provider::TransactionCount(FAddress Addr, EBlockTag Tag, TFunction<void (uint64)> OnSuccess, FFailureCallback OnFailure)
{
	const FString Content = RPCBuilder("eth_getTransactionCount").ToPtr()
		->AddArray("params").ToPtr()
			->AddString("0x" + Addr.ToHex())
			->AddValue(ConvertString(TagToString(Tag)))
			->EndArray()
		->ToString();

	const FString MyUrl = this->Url;
	SendRPCAndExtract<uint64>(Url, Content,
		OnSuccess,
		[MyUrl](FString Result)
		{
			return Provider(MyUrl).ExtractUIntResult(Result);
		},
		OnFailure);
}

void Provider::GetGasPrice(TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	const FString Content = RPCBuilder("eth_gasPrice").ToPtr()
		->AddArray("params").ToPtr()
			->EndArray()
		->ToString();

	const FString MyUrl = this->Url;
	this->SendRPCAndExtract<FUnsizedData>(Url, Content, OnSuccess, [MyUrl](FString Response)
	{
		TResult<FString> String = Provider(MyUrl).ExtractStringResult(Response);

		if(!String.HasError())
		{
			return TResult<FUnsizedData>(MakeValue(HexStringToBinary(String.GetValue())));
		}

		return TResult<FUnsizedData>(MakeError(FSequenceError(ResponseParseError, "")));
	}, OnFailure);
}

void Provider::EstimateContractCallGas(FContractCall ContractCall, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	const FString Content = RPCBuilder("eth_estimateGas").ToPtr()
			->AddArray("params").ToPtr()
				->AddValue(ContractCall.GetJson())
				->EndArray()
			->ToString();
	UE_LOG(LogTemp, Display, TEXT("My rpc call %s"), *Content);

	const FString MyUrl = this->Url;
	this->SendRPCAndExtract<FUnsizedData>(Url, Content, OnSuccess, [MyUrl](FString Response)
	{
		TResult<FString> String = Provider(MyUrl).ExtractStringResult(Response);

		if(!String.HasError())
		{
			return TResult<FUnsizedData>(MakeValue(HexStringToBinary(String.GetValue())));
		}

		return TResult<FUnsizedData>(MakeError(FSequenceError(ResponseParseError, "")));
	}, OnFailure);
}

void Provider::EstimateDeploymentGas(FAddress From, FString Bytecode, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure) //byte has ox prefix
{
	FJsonBuilder JSON = FJsonBuilder();
	JSON.AddString("from", "0x" + From.ToHex());
	JSON.AddString("data",   Bytecode);
	const FString Content = RPCBuilder("eth_estimateGas").ToPtr()
		->AddArray("params").ToPtr()
			->AddValue(JSON.ToString())
			->EndArray()
	    ->ToString();

	const FString MyUrl = this->Url;
	this->SendRPCAndExtract<FUnsizedData>(Url, Content, OnSuccess, [MyUrl](FString Response)
	{
		TResult<FString> StringResult = Provider(MyUrl).ExtractStringResult(Response);

		if(!StringResult.HasError())
		{
			return TResult<FUnsizedData>(MakeValue(HexStringToBinary(StringResult.GetValue())));
		}

		return TResult<FUnsizedData>(MakeError(FSequenceError(ResponseParseError, "")));
	}, OnFailure);
}

void Provider::DeployContractWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallbackTuple<FAddress, FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	const FAddress From = GetAddress(GetPublicKey(PrivKey));
	const FString MyUrl = this->Url;
	
	TransactionCount(From, EBlockTag::Latest, [=](uint64 Count)
	{
		const FBlockNonce Nonce = FBlockNonce::From(IntToHexString(Count));

		Provider(MyUrl).GetGasPrice([=](FUnsizedData GasPrice)
		{
			Provider(MyUrl).EstimateDeploymentGas(From, Bytecode, [=](FUnsizedData GasLimit)
			{
				const FAddress To = FAddress::From("");
				const FUnsizedData Value = HexStringToBinary("");
				const FUnsizedData Data = HexStringToBinary(Bytecode);

				FEthTransaction Transaction = FEthTransaction{Nonce, GasPrice, GasLimit, To, Value, Data};
				const FAddress DeployedAddress = GetContractAddress(From, Nonce);
				const FUnsizedData SignedTransaction = Transaction.GetSignedTransaction(PrivKey, ChainId);

				Provider(MyUrl).SendRawTransaction("0x" + SignedTransaction.ToHex(), [=](FUnsizedData Hash)
				{
					OnSuccess(DeployedAddress, Hash);
				}, OnFailure);
			}, OnFailure);
		}, OnFailure);
	}, OnFailure);
}

void Provider::DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallback<FAddress> OnSuccess, FFailureCallback OnFailure)
{
	DeployContractWithHash(Bytecode, PrivKey, ChainId, [=](FAddress Address, FUnsizedData Hash)
	{
		OnSuccess(Address);
	}, OnFailure);
}

//call method
void Provider::TransactionReceipt(FHash256 Hash, TFunction<void (FTransactionReceipt)> OnSuccess, FFailureCallback OnFailure)
{
	UE_LOG(LogTemp, Display, TEXT("My Hash is %s"), *Hash.ToHex());
	
	const FString Content = RPCBuilder("eth_getTransactionReceipt").ToPtr()
		->AddArray("params").ToPtr()
			->AddString("0x" + Hash.ToHex())
			->EndArray()
		->ToString();

	const FString MyUrl = this->Url;
	SendRPCAndExtract<FTransactionReceipt>(Url, Content,
		OnSuccess,
		[MyUrl](FString Result)
		{
			TResult<TSharedPtr<FJsonObject>> JSON = Provider(MyUrl).ExtractJsonObjectResult(Result);
			
			if(!JSON.HasError())
			{
				TResult<FTransactionReceipt> Res =  MakeValue(JsonToTransactionReceipt(JSON.GetValue()));
				return Res;
			}

			TResult<FTransactionReceipt> Res = MakeError(JSON.GetError());
			return Res;
		},
		OnFailure);
}

void Provider::NonceAt(uint64 Number, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure)
{
	return NonceAtHelper(ConvertString(ConvertInt(Number)), OnSuccess, OnFailure);
}

void Provider::NonceAt(EBlockTag Tag, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure)
{
	return NonceAtHelper(ConvertString(TagToString(Tag)), OnSuccess, OnFailure);
}

void Provider::SendRawTransaction(FString Data, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	const FString Content = RPCBuilder("eth_sendRawTransaction").ToPtr()
        ->AddArray("params").ToPtr()
	        ->AddString(Data)
	        ->EndArray()
        ->ToString();

	const FString MyUrl = this->Url;
	this->SendRPCAndExtract<FUnsizedData>(Url, Content, OnSuccess, [MyUrl](FString Response)
	{
		TResult<FString> StringResult = Provider(MyUrl).ExtractStringResult(Response);

		if(!StringResult.HasError())
		{
			return TResult<FUnsizedData>(MakeValue(HexStringToBinary(StringResult.GetValue())));
		}

		return TResult<FUnsizedData>(MakeError(FSequenceError(ResponseParseError, "")));
	}, OnFailure);
}

void Provider::ChainId(TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure)
{
	const FString Content = RPCBuilder("eth_chainId").ToString();
	FString MyUrl = this->Url;
	SendRPCAndExtract<uint64>(Url, Content, OnSuccess, [MyUrl](FString Result)
	{
		return Provider(MyUrl).ExtractUIntResult(Result);
	}, OnFailure);
}

void Provider::Call(FContractCall ContractCall, uint64 Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure) //check if eth_call
{
	return CallHelper(ContractCall, ConvertInt(Number), OnSuccess, OnFailure);
}

void Provider::Call(FContractCall ContractCall, EBlockTag Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	return CallHelper(ContractCall, ConvertString(TagToString(Number)), OnSuccess, OnFailure);
}

void Provider::NonViewCall(FEthTransaction transaction,FPrivateKey PrivateKey, int ChainID, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	FUnsizedData SignedTransaction = transaction.GetSignedTransaction(PrivateKey, ChainID);
	return SendRawTransaction("0x" + SignedTransaction.ToHex(), OnSuccess, OnFailure);
}

void Provider::CallHelper(FContractCall ContractCall, FString Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	const FString Content = RPCBuilder("eth_call").ToPtr()
		->AddArray("params").ToPtr()
			->AddValue(ContractCall.GetJson())
			->AddValue(Number)
			->EndArray()
		->ToString();

	const FString MyUrl = this->Url;
	this->SendRPCAndExtract<FUnsizedData>(Url, Content, OnSuccess, [MyUrl](FString Response)
	{
		TResult<FString> StringResult = Provider(MyUrl).ExtractStringResult(Response);

		if(!StringResult.HasError())
		{
			return TResult<FUnsizedData>(MakeValue(HexStringToBinary(StringResult.GetValue())));
		}

		return TResult<FUnsizedData>(MakeError(FSequenceError(ResponseParseError, "")));
	}, OnFailure);
}