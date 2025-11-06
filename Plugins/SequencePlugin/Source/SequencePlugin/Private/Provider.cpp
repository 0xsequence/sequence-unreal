#include "Provider.h"

#include "ConfigFetcher.h"
#include "Eth/Crypto.h"
#include "Eth/EthTransaction.h"
#include "Types/BinaryData.h"
#include "Util/HexUtility.h"
#include "Util/JsonBuilder.h"
#include "RequestHandler.h"
#include "Sequence/SequenceSdk.h"
#include "Types/Header.h"
#include "Util/Log.h"
#include "Util/SequenceSupport.h"

UProvider::UProvider()
{
	this->UpdateUrlFromConfig();
}

void UProvider::UpdateUrlFromConfig()
{
	const FString ChainPath = USequenceSupport::GetNetworkNameForUrl(SequenceSdk::GetChainId());
	const FString ProjectAccessKey = UConfigFetcher::GetConfigVar(UConfigFetcher::ProjectAccessKey);
	const FString ProviderUrl = FString::Printf(TEXT("https://nodes.sequence.app/%s/%s"), *ChainPath, *ProjectAccessKey);
	this->UpdateUrl(ProviderUrl);
}

void UProvider::UpdateUrl(const FString& UrlIn)
{
	this->Url = UrlIn;
}

void UProvider::BlockByNumberHelper(const FString& Number, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure)
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

	SendRPCAndExtract<TSharedPtr<FJsonObject>>(
		Url,
		Content,
		OnSuccess,
		[this](const FString& Json)
		{
			TResult<TSharedPtr<FJsonObject>> Obj = this->ExtractJsonObjectResult(Json);

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

void  UProvider::BlockByNumber(const uint64 Number, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure)
{
	BlockByNumberHelper(ConvertString(IntToHexString(Number)), OnSuccess, OnFailure);
}

void UProvider::BlockByNumber(const EBlockTag Tag, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure)
{
	BlockByNumberHelper(ConvertString(UEnum::GetValueAsString(Tag)), OnSuccess, OnFailure);
}

void UProvider::BlockByHash(const FHash256& Hash, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Content = RPCBuilder("eth_getBlockByHash").ToPtr()
        ->AddArray("params").ToPtr()
	        ->AddString(Hash.ToHex())
	        ->AddBool(true)
	        ->EndArray()
        ->ToString();

	SendRPCAndExtract<TSharedPtr<FJsonObject>>(Url, Content,
		OnSuccess,
		[this](const FString& Result)
		{
			return this->ExtractJsonObjectResult(Result);
		},
		OnFailure);
}

void UProvider::BlockNumber(const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Content = RPCBuilder("eth_blockNumber").ToString();
	SendRPCAndExtract<uint64>(Url, Content,
		OnSuccess,
		[this](const FString& Result)
		{
			return this->ExtractUIntResult(Result);
		},
		OnFailure);
}

void UProvider::HeaderByNumberHelper(const FString& Number, TSuccessCallback<FHeader> OnSuccess, const FFailureCallback& OnFailure)
{
	BlockByNumberHelper(Number, [OnSuccess](const TSharedPtr<FJsonObject>& Json)
	{
		OnSuccess(JsonToHeader(Json));
	}, OnFailure);
}

void UProvider::NonceAtHelper(const FString& Number, TSuccessCallback<FBlockNonce> OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<TSharedPtr<FJsonObject>> BlockCallback = [OnSuccess](const TSharedPtr<FJsonObject>& Json)
	{
		const FString Hex = Json->GetStringField(TEXT("nonce"));
		const FBlockNonce Nonce{};
		*Nonce.Arr.Get() = HexToBytesInline(Hex);
		OnSuccess(Nonce);
	};
	
	this->BlockByNumberHelper(Number, BlockCallback, OnFailure);
}

void UProvider::HeaderByNumber(const uint64 Id, const TFunction<void (FHeader)>& OnSuccess, const FFailureCallback& OnFailure)
{
	HeaderByNumberHelper(ConvertString(IntToHexString(Id)), OnSuccess, OnFailure);
}

void UProvider::HeaderByNumber(const EBlockTag Tag, const TFunction<void (FHeader)>& OnSuccess, const FFailureCallback& OnFailure)
{
	HeaderByNumberHelper(ConvertString(UEnum::GetValueAsString(Tag)), OnSuccess, OnFailure);
}

void UProvider::HeaderByHash(const FHash256& Hash, TFunction<void (FHeader)> OnSuccess, const FFailureCallback& OnFailure)
{
	BlockByHash(Hash, [OnSuccess](const TSharedPtr<FJsonObject>& Json)
	{
		const FHeader Header = JsonToHeader(Json);
		OnSuccess(Header);
	}, OnFailure); 
}

void UProvider::TransactionByHash(const FHash256& Hash, const TFunction<void (TSharedPtr<FJsonObject>)>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Content = RPCBuilder("eth_getTransactionByHash").ToPtr()
		->AddArray("params").ToPtr()
			->AddString(Hash.ToHex())
			->EndArray()
		->ToString();

	SendRPCAndExtract<TSharedPtr<FJsonObject>>(Url, Content,
		OnSuccess,
		[this](const FString& Result)
		{
			return this->ExtractJsonObjectResult(Result);
		},
		OnFailure);
}

void UProvider::TransactionCount(const FAddress& Addr, const uint64 Number, const TFunction<void (uint64)>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Content = RPCBuilder("eth_getTransactionCount").ToPtr()
		->AddArray("params").ToPtr()
			->AddString("0x" + Addr.ToHex())
			->AddValue(ConvertString(IntToHexString(Number)))
			->EndArray()
		->ToString();

	SendRPCAndExtract<uint64>(Url, Content,
		OnSuccess,
		[this](const FString& Result)
		{
			return this->ExtractUIntResult(Result);
		},
		OnFailure);
}

void UProvider::TransactionCount(const FAddress& Addr, const EBlockTag Tag, const TFunction<void (uint64)>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Content = RPCBuilder("eth_getTransactionCount").ToPtr()
		->AddArray("params").ToPtr()
			->AddString("0x" + Addr.ToHex())
			->AddValue(ConvertString(UEnum::GetValueAsString(Tag)))
			->EndArray()
		->ToString();

	SendRPCAndExtract<uint64>(Url, Content,
		OnSuccess,
		[this](const FString& Result)
		{
			return this->ExtractUIntResult(Result);
		},
		OnFailure);
}

void UProvider::GetGasPrice(const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Content = RPCBuilder("eth_gasPrice").ToPtr()
		->AddArray("params").ToPtr()
			->EndArray()
		->ToString();

	this->SendRPCAndExtract<FUnsizedData>(Url, Content, OnSuccess, [this](const FString& Response)
	{
		TResult<FString> String = this->ExtractStringResult(Response);

		if(!String.HasError())
		{
			return TResult<FUnsizedData>(MakeValue(HexStringToBinary(String.GetValue())));
		}

		return TResult<FUnsizedData>(MakeError(FSequenceError(ResponseParseError, "")));
	}, OnFailure);
}

void UProvider::EstimateContractCallGas(FContractCall ContractCall, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Content = RPCBuilder("eth_estimateGas").ToPtr()
			->AddArray("params").ToPtr()
				->AddValue(ContractCall.GetJson())
				->EndArray()
			->ToString();
	
	this->SendRPCAndExtract<FUnsizedData>(Url, Content, OnSuccess, [this](const FString& Response)
	{
		TResult<FString> String = this->ExtractStringResult(Response);

		if(!String.HasError())
		{
			return TResult<FUnsizedData>(MakeValue(HexStringToBinary(String.GetValue())));
		}

		return TResult<FUnsizedData>(MakeError(FSequenceError(ResponseParseError, "")));
	}, OnFailure);
}

void UProvider::EstimateDeploymentGas(const FAddress& From, const FString& Bytecode, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) //byte has ox prefix
{
	FJsonBuilder JSON = FJsonBuilder();
	JSON.AddString("from", "0x" + From.ToHex());
	JSON.AddString("data",   Bytecode);
	const FString Content = RPCBuilder("eth_estimateGas").ToPtr()
		->AddArray("params").ToPtr()
			->AddValue(JSON.ToString())
			->EndArray()
	    ->ToString();

	this->SendRPCAndExtract<FUnsizedData>(Url, Content, OnSuccess, [this](const FString& Response)
	{
		TResult<FString> StringResult = this->ExtractStringResult(Response);

		if(!StringResult.HasError())
		{
			return TResult<FUnsizedData>(MakeValue(HexStringToBinary(StringResult.GetValue())));
		}

		return TResult<FUnsizedData>(MakeError(FSequenceError(ResponseParseError, "")));
	}, OnFailure);
}

void UProvider::DeployContractWithHash(const FString& Bytecode, const FPrivateKey& PrivKey, const int64 ChainId, const TSuccessCallbackTuple<FAddress, FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FAddress From = GetAddress(GetPublicKey(PrivKey));
	
	TransactionCount(From, EBlockTag::ELatest, [=](uint64 Count)
	{
		const FBlockNonce Nonce = FBlockNonce::From(IntToHexString(Count));

		this->GetGasPrice([=](const FUnsizedData& GasPrice)
		{
			this->EstimateDeploymentGas(From, Bytecode, [=](const FUnsizedData& GasLimit)
			{
				const FAddress To = FAddress::From("");
				const FUnsizedData Value = HexStringToBinary("");
				const FUnsizedData Data = HexStringToBinary(Bytecode);

				FEthTransaction Transaction = FEthTransaction{Nonce, GasPrice, GasLimit, To, Value, Data};
				const FAddress DeployedAddress = GetContractAddress(From, Nonce);
				const FUnsizedData SignedTransaction = Transaction.GetSignedTransaction(PrivKey, ChainId);

				this->SendRawTransaction("0x" + SignedTransaction.ToHex(), [=](const FUnsizedData& Hash)
				{
					OnSuccess(DeployedAddress, Hash);
				}, OnFailure);
			}, OnFailure);
		}, OnFailure);
	}, OnFailure);
}

void UProvider::DeployContract(const FString& Bytecode, const FPrivateKey& PrivKey, const int64 ChainId, const TSuccessCallback<FAddress>& OnSuccess, const FFailureCallback& OnFailure)
{
	DeployContractWithHash(Bytecode, PrivKey, ChainId, [=](const FAddress& Address, FUnsizedData Hash)
	{
		OnSuccess(Address);
	}, OnFailure);
}

void UProvider::TransactionReceipt(const FHash256& Hash, const TFunction<void (FTransactionReceipt)>& OnSuccess, const FFailureCallback& OnFailure)
{	
	const FString Content = RPCBuilder("eth_getTransactionReceipt").ToPtr()
		->AddArray("params").ToPtr()
			->AddString("0x" + Hash.ToHex())
			->EndArray()
		->ToString();

	SendRPCAndExtract<FTransactionReceipt>(Url, Content,OnSuccess,[this](const FString& Result)
		{
			TResult<TSharedPtr<FJsonObject>> JSON = this->ExtractJsonObjectResult(Result);
			
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

void UProvider::CodeAt(const FString& Address, const EBlockTag BlockTag, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Content = RPCBuilder("eth_getCode").ToPtr()
		->AddArray("params").ToPtr()
			->AddString(Address)
			->AddString("pending")
			->EndArray()
		->ToString();

	this->SendRPC(Url, Content,[this, OnSuccess](const FString& Result)
	{
		const TSharedPtr<FJsonObject> Json = Parse(Result);

		if(Json == nullptr)
		{
			OnSuccess("Error parsing Json");
			return;
		}
		
		OnSuccess(Json->GetStringField(TEXT("result")));
	},
	OnFailure, false);
}

void UProvider::NonceAt(const uint64 Number, const TSuccessCallback<FBlockNonce>& OnSuccess, const FFailureCallback& OnFailure)
{
	return NonceAtHelper(ConvertString(ConvertInt(Number)), OnSuccess, OnFailure);
}

void UProvider::NonceAt(const EBlockTag Tag, const TSuccessCallback<FBlockNonce>& OnSuccess, const FFailureCallback& OnFailure)
{
	return NonceAtHelper(ConvertString(UEnum::GetValueAsString(Tag)), OnSuccess, OnFailure);
}

void UProvider::SendRawTransaction(const FString& Data, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Content = RPCBuilder("eth_sendRawTransaction").ToPtr()
        ->AddArray("params").ToPtr()
	        ->AddString(Data)
	        ->EndArray()
        ->ToString();

	this->SendRPCAndExtract<FUnsizedData>(Url, Content, OnSuccess, [this](const FString& Response)
	{
		TResult<FString> StringResult = this->ExtractStringResult(Response);

		if(!StringResult.HasError())
		{
			return TResult<FUnsizedData>(MakeValue(HexStringToBinary(StringResult.GetValue())));
		}

		return TResult<FUnsizedData>(MakeError(FSequenceError(ResponseParseError, "")));
	}, OnFailure);
}

void UProvider::ChainId(const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Content = RPCBuilder("eth_chainId").ToString();
	SendRPCAndExtract<uint64>(Url, Content, OnSuccess, [this](const FString& Result)
	{
		return this->ExtractUIntResult(Result);
	}, OnFailure);
}

void UProvider::Call(const FContractCall& ContractCall, const uint64 Number, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) //check if eth_call
{
	return CallHelper(ContractCall, ConvertInt(Number), OnSuccess, OnFailure);
}

void UProvider::Call(const FContractCall& ContractCall, const EBlockTag Number, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	return CallHelper(ContractCall, ConvertString(UEnum::GetDisplayValueAsText(Number).ToString()), OnSuccess, OnFailure);
}

void UProvider::NonViewCall(FEthTransaction Transaction, const FPrivateKey& PrivateKey, const int ChainID, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FUnsizedData SignedTransaction = Transaction.GetSignedTransaction(PrivateKey, ChainID);
	return SendRawTransaction("0x" + SignedTransaction.ToHex(), OnSuccess, OnFailure);
}

void UProvider::CallHelper(FContractCall ContractCall, const FString& Number, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Content = RPCBuilder("eth_call").ToPtr()
		->AddArray("params").ToPtr()
			->AddValue(ContractCall.GetJson())
			->AddString("pending")
			->EndArray()
		->ToString();

	this->SendRPCAndExtract<FString>(Url, Content, OnSuccess, [this](const FString& Response)
	{
		TResult<FString> StringResult = this->ExtractStringResult(Response);
		SEQ_LOG(Display, TEXT("%s"), *StringResult.GetValue());

		if(!StringResult.HasError())
		{
			return StringResult;
		}

		return TResult<FString>(MakeError(FSequenceError(ResponseParseError, "")));
	}, OnFailure);
}