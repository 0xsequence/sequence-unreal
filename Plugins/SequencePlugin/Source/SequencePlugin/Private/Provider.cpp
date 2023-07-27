// Fill out your copyright notice in the Description page of Project Settings.


#include "Provider.h"

#include "Crypto.h"
#include "EthTransaction.h"
#include "Types/BinaryData.h"
#include "HexUtility.h"
#include "HttpManager.h"
#include "JsonBuilder.h"
#include "JsonObjectConverter.h"
#include "RequestHandler.h"
#include "Transaction.h"
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

void Provider::BlockByNumberHelper(FString Number, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, TFailureCallback OnFailure)
{
	const auto Content = FJsonBuilder().ToPtr()
		->AddString("jsonrpc", "2.0")
		->AddInt("id", 1)
		->AddString("method", "eth_getBlockByNumber")
		->AddArray("params").ToPtr()
			->AddValue(Number)
			->AddBool(true)
			->EndArray()
		->ToString();

	SendRPCAndExtract<TSharedPtr<FJsonObject>>(
		Content,
		OnSuccess,
		[this](FString Json)
		{
			auto Obj = ExtractJsonObjectResult(Json);

			

			if(Obj.HasValue() && Obj.GetValue() != nullptr)
			{
				return Obj;
			}
			
			if(Obj.HasValue() && Obj.GetValue() == nullptr)
			{
				TResult<TSharedPtr<FJsonObject>> Val = MakeError(SequenceError(EmptyResponse, "Json response is null"));
				return Val;
			}

			return Obj;
		},
		OnFailure
	);
}

TSharedPtr<FJsonObject> Provider::Parse(FString JsonRaw)
{
	TSharedPtr<FJsonObject> JsonParsed;

	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		return JsonParsed;
	}

	return nullptr;
}

TResult<TSharedPtr<FJsonObject>> Provider::ExtractJsonObjectResult(FString JsonRaw)
{
	UE_LOG(LogTemp, Display, TEXT("I got this raw json: %s"), *JsonRaw);
	auto Json = Parse(JsonRaw);
	
	if(!Json)
	{
		return MakeError(SequenceError(EmptyResponse, "Could not extract response"));
	}

	return MakeValue(Json->GetObjectField("result"));
}

TResult<FString> Provider::ExtractStringResult(FString JsonRaw)
{
	auto Json = Parse(JsonRaw);
	
	if(!Json)
	{
		return MakeError(SequenceError(EmptyResponse, "Could not extract response"));
	}

	return MakeValue(Json->GetStringField("result"));
}

TResult<uint64> Provider::ExtractUIntResult(FString JsonRaw)
{
	auto Result = ExtractStringResult(JsonRaw);
	if(!Result.HasValue())
	{
		return MakeError(Result.GetError());
	}
	
	auto Convert =  HexStringToUint64(Result.GetValue());
	if(!Convert.IsSet())
	{
		return MakeError(SequenceError(ResponseParseError, "Couldn't convert \"" + Result.GetValue() + "\" to a number."));
	}
	return MakeValue(Convert.GetValue());
}

void Provider::SendRPC(FString Content, TSuccessCallback<FString> OnSuccess, TFailureCallback OnError)
{
	NewObject<URequestHandler>()
		->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/json")
		->WithVerb("POST")
		->WithContentAsString(Content)
		->ProcessAndThen(OnSuccess, OnError);
}

FJsonBuilder Provider::RPCBuilder(const FString MethodName)
{
	return *FJsonBuilder().ToPtr()
		->AddString("jsonrpc", "2.0")
		->AddInt("id", 1)
		->AddString("method", MethodName);
}

Provider::Provider(FString Url) : Url(Url)
{
}

void  Provider::BlockByNumber(uint64 Number, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, TFailureCallback OnFailure)
{
	BlockByNumberHelper(ConvertString(IntToHexString(Number)), OnSuccess, OnFailure);
}

void Provider::BlockByNumber(EBlockTag Tag, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, TFailureCallback OnFailure)
{
	BlockByNumberHelper(ConvertString(TagToString(Tag)), OnSuccess, OnFailure);
}

void Provider::BlockByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, TFailureCallback OnFailure)
{
	const auto Content = RPCBuilder("eth_getBlockByHash").ToPtr()
		->AddArray("params").ToPtr()
			->AddString(Hash.ToHex())
			->AddBool(true)
			->EndArray()
		->ToString();

	SendRPCAndExtract<TSharedPtr<FJsonObject>>(Content,
		OnSuccess,
		[this](FString Result)
		{
			return ExtractJsonObjectResult(Result);
		},
		OnFailure);
}

void Provider::BlockNumber(TSuccessCallback<uint64> OnSuccess, TFailureCallback OnFailure)
{
	const auto Content = RPCBuilder("eth_blockNumber").ToString();
	SendRPCAndExtract<uint64>(Content,
		OnSuccess,
		[this](FString Result)
		{
			return ExtractUIntResult(Result);
		},
		OnFailure);
}

void Provider::HeaderByNumberHelper(FString Number, TSuccessCallback<FHeader> OnSuccess, TFailureCallback OnFailure)
{
	BlockByNumberHelper(Number, [&OnSuccess](TSharedPtr<FJsonObject> Json)
	{
		OnSuccess(JsonToHeader(Json));
	}, OnFailure);
}

void Provider::NonceAtHelper(FString Number, TSuccessCallback<FBlockNonce> OnSuccess, TFailureCallback OnFailure)
{

	TSuccessCallback<TSharedPtr<FJsonObject>> BlockCallback = [&OnSuccess](TSharedPtr<FJsonObject> Json)
	{
		auto Hex = Json->GetStringField("nonce");
		auto Nonce = FBlockNonce::From(HexStringToHash(FBlockNonce::Size, Hex));
		OnSuccess(Nonce);
	};
	
	this->BlockByNumberHelper(Number, BlockCallback, OnFailure);
}

void Provider::HeaderByNumber(uint64 Id, TFunction<void (FHeader)> OnSuccess, TFailureCallback OnFailure)
{
	HeaderByNumberHelper(ConvertString(IntToHexString(Id)), OnSuccess, OnFailure);
}

void Provider::HeaderByNumber(EBlockTag Tag, TFunction<void (FHeader)> OnSuccess, TFailureCallback OnFailure)
{
	HeaderByNumberHelper(ConvertString(TagToString(Tag)), OnSuccess, OnFailure);
}

void Provider::HeaderByHash(FHash256 Hash, TFunction<void (FHeader)> OnSuccess, TFailureCallback OnFailure)
{
	BlockByHash(Hash, [&OnSuccess](TSharedPtr<FJsonObject> Json)
	{
		const FHeader Header = JsonToHeader(Json);
		OnSuccess(Header);
	}, OnFailure); 
}

void Provider::TransactionByHash(FHash256 Hash, TFunction<void (TSharedPtr<FJsonObject>)> OnSuccess, TFailureCallback OnFailure)
{
	const auto Content = RPCBuilder("eth_getTransactionByHash").ToPtr()
		->AddArray("params").ToPtr()
			->AddString(Hash.ToHex())
			->EndArray()
		->ToString();

	SendRPCAndExtract<TSharedPtr<FJsonObject>>(Content,
		OnSuccess,
		[this](FString Result)
		{
			return ExtractJsonObjectResult(Result);
		},
		OnFailure);
}

void Provider::TransactionCount(FAddress Addr, uint64 Number, TFunction<void (uint64)> OnSuccess, TFailureCallback OnFailure)
{
	const auto Content = RPCBuilder("eth_getTransactionCount").ToPtr()
		->AddArray("params").ToPtr()
			->AddString("0x" + Addr.ToHex())
			->AddValue(ConvertString(IntToHexString(Number)))
			->EndArray()
		->ToString();

	SendRPCAndExtract<uint64>(Content,
		OnSuccess,
		[this](FString Result)
		{
			return ExtractUIntResult(Result);
		},
		OnFailure);
}

void Provider::TransactionCount(FAddress Addr, EBlockTag Tag, TFunction<void (uint64)> OnSuccess, TFailureCallback OnFailure)
{
	const auto Content = RPCBuilder("eth_getTransactionCount").ToPtr()
		->AddArray("params").ToPtr()
			->AddString("0x" + Addr.ToHex())
			->AddValue(ConvertString(TagToString(Tag)))
			->EndArray()
		->ToString();

	SendRPCAndExtract<uint64>(Content,
		OnSuccess,
		[this](FString Result)
		{
			return ExtractUIntResult(Result);
		},
		OnFailure);
}

void Provider::GetGasPrice(TSuccessCallback<FNonUniformData> OnSuccess, TFailureCallback OnFailure)
{
	const auto Content = RPCBuilder("eth_gasPrice").ToPtr()
		->AddArray("params").ToPtr()
			->EndArray()
		->ToString();

	this->SendRPCAndExtract<FNonUniformData>(Content, OnSuccess, [this](FString Response)
	{
		auto string = ExtractStringResult(Response);

		if(!string.HasError())
		{
			return TResult<FNonUniformData>(MakeValue(HexStringToBinary(string.GetValue())));
		}

		return TResult<FNonUniformData>(MakeError(SequenceError(ResponseParseError, "")));
	}, OnFailure);
}

void Provider::EstimateContractCallGas(FContractCall ContractCall, TSuccessCallback<FNonUniformData> OnSuccess, TFailureCallback OnFailure)
{
	const auto Content = RPCBuilder("eth_estimateGas").ToPtr()
			->AddArray("params").ToPtr()
				->AddValue(ContractCall.GetJson())
				->EndArray()
			->ToString();
	UE_LOG(LogTemp, Display, TEXT("My rpc call %s"), *Content);

	this->SendRPCAndExtract<FNonUniformData>(Content, OnSuccess, [this](FString Response)
	{
		auto string = ExtractStringResult(Response);

		if(!string.HasError())
		{
			return TResult<FNonUniformData>(MakeValue(HexStringToBinary(string.GetValue())));
		}

		return TResult<FNonUniformData>(MakeError(SequenceError(ResponseParseError, "")));
	}, OnFailure);
}

void Provider::EstimateDeploymentGas(FAddress From, FString Bytecode, TSuccessCallback<FNonUniformData> OnSuccess, TFailureCallback OnFailure) //byte has ox prefix
{
	auto json = FJsonBuilder();
	json.AddString("from", "0x" + From.ToHex());
	json.AddString("data",   Bytecode);
	const auto Content = RPCBuilder("eth_estimateGas").ToPtr()
		->AddArray("params").ToPtr()
			->AddValue(json.ToString())
			->EndArray()
		->ToString();

	this->SendRPCAndExtract<FNonUniformData>(Content, OnSuccess, [this](FString Response)
	{
		auto StringResult = ExtractStringResult(Response);

		if(!StringResult.HasError())
		{
			return TResult<FNonUniformData>(MakeValue(HexStringToBinary(StringResult.GetValue())));
		}

		return TResult<FNonUniformData>(MakeError(SequenceError(ResponseParseError, "")));
	}, OnFailure);
}

void Provider::DeployContractWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, FNonUniformData* TransactionHash, TSuccessCallback<FAddress> OnSuccess, TFailureCallback OnFailure)
{
	auto FROM = GetAddress(GetPublicKey(PrivKey));

	TransactionCount(FROM, EBlockTag::Latest, [this, FROM, Bytecode, PrivKey, ChainId, TransactionHash, &OnSuccess, &OnFailure](uint64 Count)
	{
		auto NONCE = FBlockNonce::From(IntToHexString(Count));

		GetGasPrice([this, FROM, Bytecode, NONCE, PrivKey, ChainId, TransactionHash, &OnSuccess, &OnFailure](FNonUniformData GasPrice)
		{
			EstimateDeploymentGas(FROM, Bytecode, [this, GasPrice, Bytecode, NONCE, FROM, PrivKey, ChainId, TransactionHash, &OnSuccess, &OnFailure](FNonUniformData GasLimit)
			{
				auto TO = FAddress::From("");
				auto VALUE = HexStringToBinary("");
				auto DATA = HexStringToBinary(Bytecode);
				
				auto transaction = FEthTransaction{NONCE,GasPrice,GasLimit,TO,VALUE,DATA};
				FAddress DeployedAddress = GetContractAddress(FROM, NONCE);
				auto SignedTransaction = transaction.GetSignedTransaction(PrivKey, ChainId);

				SendRawTransaction("0x" + SignedTransaction.ToHex(), [TransactionHash, &OnSuccess, DeployedAddress](FNonUniformData Data)
				{
					if(TransactionHash != nullptr)
					{
						*TransactionHash = Data;
					}
					OnSuccess(DeployedAddress);
				}, OnFailure);
			}, OnFailure);
		}, OnFailure);
	}, OnFailure);
}

void Provider::DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallback<FAddress> OnSuccess, TFailureCallback OnFailure)
{
	DeployContractWithHash(Bytecode, PrivKey, ChainId, nullptr, OnSuccess, OnFailure);
}

//call method
void Provider::TransactionReceipt(FHash256 Hash, TFunction<void (FTransactionReceipt)> OnSuccess, TFailureCallback OnFailure)
{
	const auto Content = RPCBuilder("eth_getTransactionReceipt").ToPtr()
		->AddArray("params").ToPtr()
			->AddString("0x" + Hash.ToHex())
			->EndArray()
		->ToString();

	SendRPCAndExtract<FTransactionReceipt>(Content,
		OnSuccess,
		[this](FString Result)
		{
			auto json = ExtractJsonObjectResult(Result);
			
			if(!json.HasError())
			{
				TResult<FTransactionReceipt> Res =  MakeValue(JsonToTransactionReceipt(json.GetValue()));
				return Res;
			}

			TResult<FTransactionReceipt> Res = MakeError(json.GetError());
			return Res;
		},
		OnFailure);
}

void Provider::NonceAt(uint64 Number, TSuccessCallback<FBlockNonce> OnSuccess, TFailureCallback OnFailure)
{
	return NonceAtHelper(ConvertString(ConvertInt(Number)), OnSuccess, OnFailure);
}

void Provider::NonceAt(EBlockTag Tag, TSuccessCallback<FBlockNonce> OnSuccess, TFailureCallback OnFailure)
{
	return NonceAtHelper(ConvertString(TagToString(Tag)), OnSuccess, OnFailure);
}

void Provider::SendRawTransaction(FString Data, TSuccessCallback<FNonUniformData> OnSuccess, TFailureCallback OnFailure)
{
	const auto Content = RPCBuilder("eth_sendRawTransaction").ToPtr()
		->AddArray("params").ToPtr()
			->AddString(Data)
			->EndArray()
		->ToString();

	this->SendRPCAndExtract<FNonUniformData>(Content, OnSuccess, [this](FString Response)
	{
		auto StringResult = ExtractStringResult(Response);

		if(!StringResult.HasError())
		{
			return TResult<FNonUniformData>(MakeValue(HexStringToBinary(StringResult.GetValue())));
		}

		return TResult<FNonUniformData>(MakeError(SequenceError(ResponseParseError, "")));
	}, OnFailure);
}

void Provider::ChainId(TSuccessCallback<uint64> OnSuccess, TFailureCallback OnFailure)
{
	const FString Content = RPCBuilder("eth_chainId").ToString();
	SendRPCAndExtract<uint64>(Content, OnSuccess, [this](FString Result)
	{
		return ExtractUIntResult(Result);
	}, OnFailure);
}

void Provider::Call(FContractCall ContractCall, uint64 Number, TSuccessCallback<FNonUniformData> OnSuccess, TFailureCallback OnFailure) //check if eth_call
{
	return CallHelper(ContractCall, ConvertInt(Number), OnSuccess, OnFailure);
}

void Provider::Call(FContractCall ContractCall, EBlockTag Number, TSuccessCallback<FNonUniformData> OnSuccess, TFailureCallback OnFailure)
{
	return CallHelper(ContractCall, ConvertString(TagToString(Number)), OnSuccess, OnFailure);
}

void Provider::NonViewCall(FEthTransaction transaction,FPrivateKey PrivateKey, int ChainID, TSuccessCallback<FNonUniformData> OnSuccess, TFailureCallback OnFailure)
{
	auto SignedTransaction = transaction.GetSignedTransaction(PrivateKey, ChainID);
	return SendRawTransaction("0x" + SignedTransaction.ToHex(), OnSuccess, OnFailure);
}

void Provider::CallHelper(FContractCall ContractCall, FString Number, TSuccessCallback<FNonUniformData> OnSuccess, TFailureCallback OnFailure)
{
	const auto Content = RPCBuilder("eth_call").ToPtr()
		->AddArray("params").ToPtr()
			->AddValue(ContractCall.GetJson())
			->AddValue(Number)
			->EndArray()
		->ToString();

	this->SendRPCAndExtract<FNonUniformData>(Content, OnSuccess, [this](FString Response)
	{
		auto StringResult = ExtractStringResult(Response);

		if(!StringResult.HasError())
		{
			return TResult<FNonUniformData>(MakeValue(HexStringToBinary(StringResult.GetValue())));
		}

		return TResult<FNonUniformData>(MakeError(SequenceError(ResponseParseError, "")));
	}, OnFailure);
}