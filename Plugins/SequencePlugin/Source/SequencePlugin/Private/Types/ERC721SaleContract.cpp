#include "Types/ERC721SaleContract.h"
#include "ABI/ABI.h"

FRawTransaction UERC721SaleContract::MakePurchaseTransaction(const FString& ToAddress, const int32& Amount, const TArray<FString>& Proof)
{
    FString FunctionSignature = "mint(address,uint256,address,uint256,bytes32[])";

    FString ToAddressNoPrefix = ToAddress.Mid(2, ToAddress.Len());
    FAddress ToAddressBytes = FAddress::From(ToAddressNoPrefix);
    TFixedABIData ABIAccountTo = ABI::Address(ToAddressBytes);

    TFixedABIData ABIAmount = ABI::Int32(Amount);

    FString PaymentTokenNoPrefix = PaymentToken.Mid(2, PaymentToken.Len());
    FAddress PaymentTokenBytes = FAddress::From(PaymentTokenNoPrefix);
    TFixedABIData ABIAddressPayment = ABI::Address(PaymentTokenBytes);

    TFixedABIData ABIMaxTotal = ABI::Int32(MaxTotal);

    TArray<ABIEncodeable*> ProofArray;
    for (const FString& ProofEntry : Proof)
    {
        TArray<uint8> ProofBytes;
        ProofBytes.SetNumUninitialized(32);
        FMemory::Memcpy(ProofBytes.GetData(), TCHAR_TO_UTF8(*ProofEntry), FMath::Min(ProofEntry.Len(), 32));
        ProofArray.Add(new TFixedABIData(ProofBytes));
    }
    TDynamicABIArray ABIArrayProof(ProofArray);

    TArray<ABIEncodeable*> Arr;
    Arr.Add(&ABIAccountTo);      
    Arr.Add(&ABIAmount);         
    Arr.Add(&ABIAddressPayment); 
    Arr.Add(&ABIMaxTotal);       
    Arr.Add(&ABIArrayProof);     

    FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

    FRawTransaction T;
    T.data = "0x" + EncodedData.ToHex();
    T.to = ContractAddress;
    T.value = "0";
   
    return T; 
}
