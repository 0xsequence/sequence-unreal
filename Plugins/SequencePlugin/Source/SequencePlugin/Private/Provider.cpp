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

TFuture<TResult<TSharedPtr<FJsonObject>>> Provider::GetBlockByNumberHelper(FString Number)
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
	
	return Async(EAsyncExecution::Thread, [this, Content]
	{
		return ExtractJsonObjectResult(SendRPC(Content).Get());
	});
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

TFuture<FString> Provider::SendRPC(FString Content)
{
	return NewObject<URequestHandler>()
		->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/json")
		->WithVerb("POST")
		->WithContentAsString(Content)
		->Process();
}

TFuture<TResult<uint64>> Provider::TransactionCountHelper(FAddress Address, FString Number)
{
	const auto Content = RPCBuilder("eth_getTransactionCount").ToPtr()
		->AddArray("params").ToPtr()
			->AddString("0x" + Address.ToHex())
			->AddValue(Number)
			->EndArray()
		->ToString();
	
	return Async(EAsyncExecution::Thread, [Content, this]
	{
		return ExtractUIntResult(SendRPC(Content).Get());
	});
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

TFuture<TResult<TSharedPtr<FJsonObject>>>  Provider::BlockByNumber(uint64 Id)
{
	return GetBlockByNumberHelper(ConvertString(IntToHexString(Id)));
}

TFuture<TResult<TSharedPtr<FJsonObject>>> Provider::BlockByNumber(EBlockTag Tag)
{
	return GetBlockByNumberHelper(ConvertString(TagToString(Tag)));
}

TFuture<TResult<TSharedPtr<FJsonObject>>> Provider::BlockByHash(FHash256 Hash)
{
	const auto Content = RPCBuilder("eth_getBlockByHash").ToPtr()
		->AddArray("params").ToPtr()
			->AddString(Hash.ToHex())
			->AddBool(true)
			->EndArray()
		->ToString();
	return Async(EAsyncExecution::Thread, [this, Content]
	{
		return ExtractJsonObjectResult(SendRPC(Content).Get());
	});
}

TFuture<TResult<uint64>> Provider::BlockNumber()
{
	const auto Content = RPCBuilder("eth_blockNumber").ToString();
	return Async(EAsyncExecution::Thread, [this, Content]
	{
		return ExtractUIntResult(SendRPC(Content).Get());
	});
}

TResult<FHeader> Provider::HeaderByNumberHelperSynchronous(FString Number)
{
	const auto Content = GetBlockByNumberHelper(Number).Get();

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

TFuture<TResult<FHeader>> Provider::HeaderByNumberHelper(FString Number)
{
	return Async(EAsyncExecution::Thread, [this, Number]
	{
		return HeaderByNumberHelperSynchronous(Number);
	});
}

TResult<FBlockNonce> Provider::NonceAtHelperSynchronous(FString Number)
{
	const auto Content = GetBlockByNumberHelper(Number).Get();

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


TFuture<TResult<FBlockNonce>> Provider::NonceAtHelper(FString Number)
{
	return Async(EAsyncExecution::Thread, [this, Number]
	{
		return NonceAtHelperSynchronous(Number);
	});
}

TFuture<TResult<FHeader>> Provider::HeaderByNumber(uint64 Id)
{
	return HeaderByNumberHelper(ConvertString(IntToHexString(Id)));
}

TFuture<TResult<FHeader>> Provider::HeaderByNumber(EBlockTag Tag)
{
	return HeaderByNumberHelper(ConvertString(TagToString(Tag)));
}

TFuture<TResult<FHeader>> Provider::HeaderByHash(FHash256 Hash)
{
	return Async(EAsyncExecution::Thread, [this, Hash]
	{
		return HeaderByHashSynchronous(Hash);
	});
}

TFuture<TResult<TSharedPtr<FJsonObject>>> Provider::TransactionByHash(FHash256 Hash)
{
	const auto Content = RPCBuilder("eth_getTransactionByHash").ToPtr()
		->AddArray("params").ToPtr()
			->AddString(Hash.ToHex())
			->EndArray()
		->ToString();
	
	return Async(EAsyncExecution::Thread, [this, Content]()
	{
		return ExtractJsonObjectResult(SendRPC(Content).Get());
	});
}

TFuture<TResult<uint64>> Provider::TransactionCount(FAddress Addr, uint64 Number)
{
	return TransactionCountHelper(Addr, ConvertInt(Number));
}

TFuture<TResult<uint64>> Provider::TransactionCount(FAddress Addr, EBlockTag Tag)
{
	return TransactionCountHelper(Addr, ConvertString(TagToString(Tag)));
}

TFuture<TResult<FNonUniformData>> Provider::GetGasPrice()
{
	return Async(EAsyncExecution::Thread, [this]
	{
		return GetGasPriceSynchronous();
	});
}

TFuture<TResult<FNonUniformData>> Provider::EstimateContractCallGas(FContractCall ContractCall)
{
	return Async(EAsyncExecution::Thread, [this, ContractCall]()
	{
		return EstimateContractCallGasSynchronous(ContractCall);
	});
}

TResult<FNonUniformData> Provider::EstimateDeploymentGasSynchronous(FAddress from, FString Bytecode)
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
	auto Data = ExtractStringResult(SendRPC(Content).Get());
	if(Data.HasError())
	{
		return MakeError(Data.GetError());
	}
	
	return MakeValue(HexStringToBinary(Data.GetValue()));
}


TFuture<TResult<FNonUniformData>> Provider::EstimateDeploymentGas(FAddress from, FString Bytecode) //byte has ox prefix
{
	return Async(EAsyncExecution::Thread, [from, Bytecode, this]
	{
		return EstimateDeploymentGasSynchronous(from, Bytecode);
	});
}

FAddress Provider::DeployContractSynchronousWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, FNonUniformData* TransactionHash)
{
	auto FROM = GetAddress(GetPublicKey(PrivKey));
	auto NONCE = FBlockNonce::From(IntToHexString(TransactionCount(FROM, EBlockTag::Latest).Get().GetValue()));
	auto GASPRICE = GetGasPrice().Get().GetValue();
	auto GASLIMIT = EstimateDeploymentGas(FROM, Bytecode).Get().GetValue();
	auto TO = FAddress::From("");
	auto VALUE = HexStringToBinary("");
	auto DATA = HexStringToBinary(Bytecode);
	
	auto transaction = FEthTransaction{NONCE,GASPRICE,GASLIMIT,TO,VALUE,DATA};
	FAddress DeployedAddress = GetContractAddress(FROM, NONCE);
	auto SignedTransaction = transaction.GetSignedTransaction(PrivKey, ChainId);
	auto TransactionResult = SendRawTransaction("0x" + SignedTransaction.ToHex()).Get();

	if(TransactionResult.HasValue())
	{
		*TransactionHash = TransactionResult.GetValue();
	}
	return DeployedAddress;
}

TFuture<FAddress> Provider::DeployContractWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, FNonUniformData* TransactionHash)
{
	return Async(EAsyncExecution::Thread, [this, Bytecode, PrivKey, ChainId, TransactionHash]
	{
		return DeployContractSynchronousWithHash(Bytecode, PrivKey, ChainId, TransactionHash);
	});
}

TFuture<FAddress> Provider::DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId)
{
	return Async(EAsyncExecution::Thread, [this, Bytecode, PrivKey, ChainId]
	{
		return DeployContractSynchronous(Bytecode, PrivKey, ChainId);
	});
}



FAddress Provider::DeployContractSynchronous(FString Bytecode, FPrivateKey PrivKey, int64 ChainId)
{
	auto FROM = GetAddress(GetPublicKey(PrivKey));
	auto NONCE = FBlockNonce::From(IntToHexString(TransactionCount(FROM, EBlockTag::Latest).Get().GetValue()));
	auto GASPRICE = GetGasPrice().Get().GetValue();
	auto GASLIMIT = EstimateDeploymentGas(FROM, Bytecode).Get().GetValue();
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
TFuture<TResult<FTransactionReceipt>> Provider::TransactionReceipt(FHash256 Hash)
{
	return Async(EAsyncExecution::Thread, [this, Hash]
	{
		return TransactionReceiptSynchronous(Hash);
	});
}


TResult<FTransactionReceipt> Provider::TransactionReceiptSynchronous(FHash256 Hash)
{
	const auto Content = RPCBuilder("eth_getTransactionReceipt").ToPtr()
		->AddArray("params").ToPtr()
			->AddString("0x" + Hash.ToHex())
			->EndArray()
		->ToString();
	auto Json = ExtractJsonObjectResult(SendRPC(Content).Get());
	if(Json.HasError())
	{
		return MakeError(Json.GetError());
	}
	return MakeValue(JsonToTransactionReceipt(Json.GetValue()));
}

TFuture<TResult<FBlockNonce>> Provider::NonceAt(uint64 Number)
{
	return NonceAtHelper(ConvertInt(Number));
}

TFuture<TResult<FBlockNonce>> Provider::NonceAt(EBlockTag Tag)
{
	return NonceAtHelper(ConvertString(TagToString(Tag)));
}

TFuture<TResult<FNonUniformData>> Provider::SendRawTransaction(FString data)
{
	return Async(EAsyncExecution::Thread, [this, data]()
	{
		return SendRawTransactionSynchronous(data);
	});
}

TResult<uint64> Provider::ChainIdSynchronous()
{
	const auto Content = RPCBuilder("eth_chainId").ToString();
	return ExtractUIntResult(SendRPC(Content).Get());
}

TResult<FNonUniformData> Provider::SendRawTransactionSynchronous(FString data)
{
	const auto Content = RPCBuilder("eth_sendRawTransaction").ToPtr()
		->AddArray("params").ToPtr()
			->AddString(data)
			->EndArray()
		->ToString();

	auto Data = ExtractStringResult(SendRPC(Content).Get());

	if(Data.HasError())
	{
		return MakeError(Data.GetError());
	}
	
	return MakeValue(HexStringToBinary(Data.GetValue()));
}

TFuture<TResult<uint64>> Provider::ChainId()
{
	return Async(EAsyncExecution::Thread, [this]
	{
		return ChainIdSynchronous();
	});
}

TFuture<TResult<FNonUniformData>> Provider::Call(FContractCall ContractCall, uint64 Number) //check if eth_call
{
	return CallHelper(ContractCall, ConvertInt(Number));
}

TFuture<TResult<FNonUniformData>> Provider::Call(FContractCall ContractCall, EBlockTag Number)
{
	return CallHelper(ContractCall, ConvertString(TagToString(Number)));
}

TFuture<TResult<FNonUniformData>> Provider::NonViewCall(FEthTransaction transaction,FPrivateKey PrivateKey, int ChainID  )
{
	auto SignedTransaction = transaction.GetSignedTransaction(PrivateKey, ChainID);
	return SendRawTransaction("0x" + SignedTransaction.ToHex());
}

TResult<FNonUniformData> Provider::CallHelperSynchronous(FContractCall ContractCall, FString Number)
{
	const auto Content = RPCBuilder("eth_call").ToPtr()
		->AddArray("params").ToPtr()
			->AddValue(ContractCall.GetJson())
			->AddValue(Number)
			->EndArray()
		->ToString();
	UE_LOG(LogTemp, Display, TEXT("My rpc call %s"), *Content);
	auto Data = ExtractStringResult(SendRPC(Content).Get());

	if(Data.HasError())
	{
		return MakeError(Data.GetError());
	}
	
	return MakeValue(HexStringToBinary(Data.GetValue()));
}

TFuture<TResult<FNonUniformData>> Provider::CallHelper(FContractCall ContractCall, FString Number)
{
	return Async(EAsyncExecution::Thread, [ContractCall, Number, this]
	{
			return CallHelperSynchronous(ContractCall, Number);
	});
}


TResult<FHeader> Provider::HeaderByHashSynchronous(FHash256 Hash)
{
	const auto Content = BlockByHash(Hash).Get();

	if(Content.HasError())
	{
		return MakeError(Content.GetError());
	}

	auto Obj = Content.GetValue();
	auto Header = JsonToHeader(Obj);

	return MakeValue(Header);
}

TResult<FNonUniformData> Provider::GetGasPriceSynchronous()
{
	const auto Content = RPCBuilder("eth_gasPrice").ToPtr()
		->AddArray("params").ToPtr()
			->EndArray()
		->ToString();
	
	auto Data = ExtractStringResult(SendRPC(Content).Get());

	if(Data.HasError())
	{
		return MakeError(Data.GetError());
	}
	
	return MakeValue(HexStringToBinary(Data.GetValue()));
}

TResult<FNonUniformData> Provider::EstimateContractCallGasSynchronous(FContractCall ContractCall)
{
	const auto Content = RPCBuilder("eth_estimateGas").ToPtr()
			->AddArray("params").ToPtr()
				->AddValue(ContractCall.GetJson())
				->EndArray()
			->ToString();
	UE_LOG(LogTemp, Display, TEXT("My rpc call %s"), *Content);
	auto Data = ExtractStringResult(SendRPC(Content).Get());
	if(Data.HasError())
	{
		return MakeError(Data.GetError());
	}
	
	return MakeValue(HexStringToBinary(Data.GetValue()));
}
