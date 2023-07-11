// Fill out your copyright notice in the Description page of Project Settings.


#include "Provider.h"

#include "Crypto.h"
#include "FEthTransaction.h"
#include "Types/BinaryData.h"
#include "HexUtility.h"
#include "HttpManager.h"
#include "JsonBuilder.h"
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

TResult<TSharedPtr<FJsonObject>> Provider::GetBlockByNumberHelper(FString Number)
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
	return ExtractJsonObjectResult(SendRPC(Content));
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

FString Provider::SendRPC(FString Content)
{
	auto responseContent = NewObject<URequestHandler>()
		->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/json")
		->WithVerb("POST")
		->WithContentAsString(Content)
		->Process()
		->GetContentAsString();

	 UE_LOG(LogTemp, Display, TEXT("Received response: %s"), *responseContent);

	return responseContent;
}

TResult<uint64> Provider::TransactionCountHelper(FAddress Address, FString Number)
{
	const auto Content = RPCBuilder("eth_getTransactionCount").ToPtr()
		->AddArray("params").ToPtr()
			->AddString("0x" + Address.ToHex())
			->AddValue(Number)
			->EndArray()
		->ToString();
	return ExtractUIntResult(SendRPC(Content));
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

TResult<TSharedPtr<FJsonObject>>  Provider::BlockByNumber(uint64 Id)
{
	return GetBlockByNumberHelper(ConvertString(IntToHexString(Id)));
}

TResult<TSharedPtr<FJsonObject>> Provider::BlockByNumber(EBlockTag Tag)
{
	return GetBlockByNumberHelper(ConvertString(TagToString(Tag)));
}

TResult<TSharedPtr<FJsonObject>> Provider::BlockByHash(FHash256 Hash)
{
	const auto Content = RPCBuilder("eth_getBlockByHash").ToPtr()
		->AddArray("params").ToPtr()
			->AddString(Hash.ToHex())
			->AddBool(true)
			->EndArray()
		->ToString();
	return ExtractJsonObjectResult(SendRPC(Content));
}

TResult<uint64> Provider::BlockNumber()
{
	const auto Content = RPCBuilder("eth_blockNumber").ToString();
	return ExtractUIntResult(SendRPC(Content));
}

TResult<FHeader> Provider::HeaderByNumberHelper(FString Number)
{
	const auto Content = GetBlockByNumberHelper(Number);

	if(Content.HasError())
	{
		return MakeError(Content.GetError());
	}

	auto Obj = Content.GetValue();

	if(Obj == nullptr)
	{
		return MakeError(SequenceError(EmptyResponse, "Json response is null"));
	}
	
	auto Header = JsonToHeader(Obj);

	return MakeValue(Header);
}

TResult<FBlockNonce> Provider::NonceAtHelper(FString Number)
{
	const auto Content = GetBlockByNumberHelper(Number);

	if(Content.HasError())
	{
		return MakeError(Content.GetError());
	}

	auto Obj = Content.GetValue();

	if(Obj == nullptr)
	{
		return MakeError(SequenceError(EmptyResponse, "Response is null"));
	}
	
	auto Hex = Obj->GetStringField("nonce");
	return MakeValue(FBlockNonce::From(HexStringToHash(FBlockNonce::Size, Hex)));
}

TResult<FHeader> Provider::HeaderByNumber(uint64 Id)
{
	return HeaderByNumberHelper(ConvertString(IntToHexString(Id)));
}

TResult<FHeader> Provider::HeaderByNumber(EBlockTag Tag)
{
	return HeaderByNumberHelper(ConvertString(TagToString(Tag)));
}

TResult<FHeader> Provider::HeaderByHash(FHash256 Hash)
{
	const auto Content = BlockByHash(Hash);

	if(Content.HasError())
	{
		return MakeError(Content.GetError());
	}

	auto Obj = Content.GetValue();
	auto Header = JsonToHeader(Obj);

	return MakeValue(Header);
}

TResult<TSharedPtr<FJsonObject>> Provider::TransactionByHash(FHash256 Hash)
{
	const auto Content = RPCBuilder("eth_getTransactionByHash").ToPtr()
		->AddArray("params").ToPtr()
			->AddString(Hash.ToHex())
			->EndArray()
		->ToString();
	return ExtractJsonObjectResult(SendRPC(Content));
}

TResult<uint64> Provider::TransactionCount(FAddress Addr, uint64 Number)
{
	return TransactionCountHelper(Addr, ConvertInt(Number));
}

TResult<uint64> Provider::TransactionCount(FAddress Addr, EBlockTag Tag)
{
	return TransactionCountHelper(Addr, ConvertString(TagToString(Tag)));
}

TResult<FNonUniformData> Provider::getGasPrice()
{
	const auto Content = RPCBuilder("eth_gasPrice").ToPtr()
		->AddArray("params").ToPtr()
			->EndArray()
		->ToString();
	
	auto Data = ExtractStringResult(SendRPC(Content));

	if(Data.HasError())
	{
		return MakeError(Data.GetError());
	}
	
	return MakeValue(HexStringToBinary(Data.GetValue()));
	
}

TResult<FNonUniformData> Provider::estimateContractCallGas(ContractCall ContractCall)
{
	{
		const auto Content = RPCBuilder("eth_estimateGas").ToPtr()
			->AddArray("params").ToPtr()
				->AddValue(ContractCall.GetJson())
				->EndArray()
			->ToString();
		UE_LOG(LogTemp, Display, TEXT("My rpc call %s"), *Content);
		auto Data = ExtractStringResult(SendRPC(Content));
		if(Data.HasError())
		{
			return MakeError(Data.GetError());
		}
		return MakeValue(HexStringToBinary(Data.GetValue()));
	}
}

TResult<FNonUniformData> Provider::estimateDeploymentGas(FAddress from, FString Bytecode) //byte has ox prefix
{
		auto json = FJsonBuilder();
		json.AddString("from", "0x" + from.ToHex());
		json.AddString("data",   Bytecode);
		const auto Content = RPCBuilder("eth_estimateGas").ToPtr()
			->AddArray("params").ToPtr()
				->AddValue(json.ToString())
				->EndArray()
			->ToString();
		UE_LOG(LogTemp, Display, TEXT("My rpc call %s"), *Content);
		auto Data = ExtractStringResult(SendRPC(Content));
		if(Data.HasError())
		{
			return MakeError(Data.GetError());
		}
		return MakeValue(HexStringToBinary(Data.GetValue()));
}

FAddress Provider::DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TResult<FNonUniformData>& TransactionHash)
{
	auto FROM = GetAddress(GetPublicKey(PrivKey));
	auto NONCE = FBlockNonce::From(IntToHexString(TransactionCount(FROM, EBlockTag::Latest).GetValue()));
	auto GASPRICE = getGasPrice().GetValue();
	auto GASLIMIT = estimateDeploymentGas(FROM, Bytecode).GetValue();
	auto TO = FAddress::From("");
	auto VALUE = HexStringToBinary("");
	auto DATA = HexStringToBinary(Bytecode);
	
	auto transaction = FEthTransaction{NONCE,GASPRICE,GASLIMIT,TO,VALUE,DATA};
	FAddress DeployedAddress = GetContractAddress(FROM, NONCE);
	auto SignedTransaction = transaction.GetSignedTransaction(PrivKey, ChainId);
	TransactionHash = SendRawTransaction("0x" + SignedTransaction.ToHex());
	return DeployedAddress;
}

FAddress Provider::DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId)
{
	auto FROM = GetAddress(GetPublicKey(PrivKey));
	auto NONCE = FBlockNonce::From(IntToHexString(TransactionCount(FROM, EBlockTag::Latest).GetValue()));
	auto GASPRICE = getGasPrice().GetValue();
	auto GASLIMIT = estimateDeploymentGas(FROM, Bytecode).GetValue();
	auto TO = FAddress::From("");
	auto VALUE = HexStringToBinary("");
	auto DATA = HexStringToBinary(Bytecode);
	
	auto transaction = FEthTransaction{
		NONCE,
		GASPRICE,
		GASLIMIT,
		TO,
		VALUE,
		DATA,
	};
	FAddress DeployedAddress = GetContractAddress(FROM, NONCE);
	auto SignedTransaction = transaction
	.GetSignedTransaction(PrivKey, ChainId);
	auto Retval = SendRawTransaction(
		"0x" + SignedTransaction.ToHex()
	 );
	return DeployedAddress;
}

//call method

TResult<FTransactionReceipt> Provider::TransactionReceipt(FHash256 Hash)
{
	const auto Content = RPCBuilder("eth_getTransactionReceipt").ToPtr()
		->AddArray("params").ToPtr()
			->AddString("0x" + Hash.ToHex())
			->EndArray()
		->ToString();
	auto Json = ExtractJsonObjectResult(SendRPC(Content));
	if(Json.HasError())
	{
		return MakeError(Json.GetError());
	}
	return MakeValue(JsonToTransactionReceipt(Json.GetValue()));
}

TResult<FBlockNonce> Provider::NonceAt(uint64 Number)
{
	return NonceAtHelper(ConvertInt(Number));
}

TResult<FBlockNonce> Provider::NonceAt(EBlockTag Tag)
{
	return NonceAtHelper(ConvertString(TagToString(Tag)));
}

TResult<FNonUniformData> Provider::SendRawTransaction(FString data)
{
	const auto Content = RPCBuilder("eth_sendRawTransaction").ToPtr()
		->AddArray("params").ToPtr()
			->AddString(data)
			->EndArray()
		->ToString();

	auto Data = ExtractStringResult(SendRPC(Content));

	if(Data.HasError())
	{
		return MakeError(Data.GetError());
	}
	
	return MakeValue(HexStringToBinary(Data.GetValue()));
}

TResult<uint64> Provider::ChainId()
{
	const auto Content = RPCBuilder("eth_chainId").ToString();
	return ExtractUIntResult(SendRPC(Content));
}

TResult<FNonUniformData> Provider::Call(ContractCall ContractCall, uint64 Number) //check if eth_call
{
	return CallHelper(ContractCall, ConvertInt(Number));
}

TResult<FNonUniformData> Provider::Call(ContractCall ContractCall, EBlockTag Number)
{
	return CallHelper(ContractCall, ConvertString(TagToString(Number)));
}

TResult<FNonUniformData> Provider::NonViewCall(FEthTransaction transaction,FPrivateKey PrivateKey, int ChainID  )
{
	auto SignedTransaction = transaction
		.GetSignedTransaction(PrivateKey, ChainID);

	return SendRawTransaction("0x" + SignedTransaction.ToHex());
	
}

TResult<FNonUniformData> Provider::CallHelper(ContractCall ContractCall, FString Number)
{
	const auto Content = RPCBuilder("eth_call").ToPtr()
		->AddArray("params").ToPtr()
			->AddValue(ContractCall.GetJson())
			->AddValue(Number)
			->EndArray()
		->ToString();
	UE_LOG(LogTemp, Display, TEXT("My rpc call %s"), *Content);
	auto Data = ExtractStringResult(SendRPC(Content));

	if(Data.HasError())
	{
		return MakeError(Data.GetError());
	}
	
	return MakeValue(HexStringToBinary(Data.GetValue()));
	
}
