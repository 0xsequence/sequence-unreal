#include "Types/ERC721SaleContract.h"
#include "ABI/ABI.h"
#include "Util/Log.h"

UERC721SaleContract::UERC721SaleContract()
{
}

UERC721SaleContract* UERC721SaleContract::Create(const FString& ContractAddress, const FString& PaymentToken, int32 MaxTotal, const FString& Data)
{
    UERC721SaleContract* Contract = NewObject<UERC721SaleContract>();
    Contract->Init(ContractAddress, PaymentToken, MaxTotal, Data);
    return Contract;
}

void UERC721SaleContract::Init(const FString& InContractAddress, const FString& InPaymentToken, int32 InMaxTotal, const FString& InData)
{
    this->ContractAddress = InContractAddress;
    this->PaymentToken = InPaymentToken;
    this->MaxTotal = InMaxTotal;
    this->Data = InData;
}

FRawTransaction UERC721SaleContract::MakePurchaseTransaction(const FString& ToAddress, const int32& Amount, const TArray<FString>& Proof)
{
    FString FunctionSignature = "mint(address,uint256,address,uint256,bytes32[])";

    FString ToAddressNoPrefix = ToAddress.Mid(2, ToAddress.Len());
    FAddress ToAddressBytes = FAddress::From(ToAddressNoPrefix);
    TSharedPtr<TFixedABIData> ABIAccountTo = MakeShared<TFixedABIData>(ABI::Address(ToAddressBytes));

    TSharedPtr<TFixedABIData> ABIAmount = MakeShared<TFixedABIData>(ABI::Int32(Amount)); 

    FString PaymentTokenNoPrefix = PaymentToken.Mid(2, PaymentToken.Len());
    FAddress PaymentTokenBytes = FAddress::From(PaymentTokenNoPrefix);
    TSharedPtr<TFixedABIData> ABIAddressPayment = MakeShared<TFixedABIData>(ABI::Address(PaymentTokenBytes));

    TSharedPtr<TFixedABIData> ABIMaxTotal = MakeShared<TFixedABIData>(ABI::Int32(MaxTotal));

    TArray<TSharedPtr<ABIElement>> ProofArray;
    for (const FString& ProofEntry : Proof)
    {
        TArray<uint8> ProofBytes;
        ProofBytes.SetNumUninitialized(32);
        FMemory::Memcpy(ProofBytes.GetData(), TCHAR_TO_UTF8(*ProofEntry), FMath::Min(ProofEntry.Len(), 32));

        const TFixedABIData* ProofData = new TFixedABIData(ProofBytes);
        ProofArray.Add(MakeShared<TFixedABIData>(*ProofData));
    }
    TSharedPtr<TDynamicABIArray> ABIArrayProof = MakeShared<TDynamicABIArray>(ProofArray);

    TArray<TSharedPtr<ABIElement>> Arr;
    Arr.Add(ABIAccountTo);      
    Arr.Add(ABIAmount);         
    Arr.Add(ABIAddressPayment); 
    Arr.Add(ABIMaxTotal);       
    Arr.Add(ABIArrayProof);     

    TOptional<FUnsizedData> EncodedData = ABI::Encode(FunctionSignature, Arr);
    FRawTransaction T;
	
    if (!EncodedData.IsSet())
    {
        SEQ_LOG(Display, TEXT("Encoded data is invalid"));
        return T;
    }

    T.data = "0x" + EncodedData.GetValue().ToHex();
    T.to = ContractAddress;
    T.value = "0";
   
    return T; 
}


FContractCall UERC721SaleContract::GetSaleDetails()
{
    FString FunctionSignature = "saleDetails()";
    TArray<ABIElement*> Arr;
    FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

    FContractCall CallData;
    CallData.Data = TOptional(EncodedData.ToHex());
    CallData.To = FAddress::From(ContractAddress);

    return CallData;
}