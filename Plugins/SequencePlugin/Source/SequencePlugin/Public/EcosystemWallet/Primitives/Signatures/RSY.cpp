#include "RSY.h"
#include "EthAbi/EthAbiBridge.h"
#include "Util/ByteArrayUtils.h"

TArray<uint8> FRSY::Pack() const
{
    checkf(YParity == 0 || YParity == 1, TEXT("YParity must be 0 or 1"));

    TArray<uint8> RBytes = FByteArrayUtils::PadLeft(FEthAbiBridge::EncodeBigInteger(R), 32);
    TArray<uint8> SBytes = FByteArrayUtils::PadLeft(FEthAbiBridge::EncodeBigInteger(S), 32);

    if (YParity % 2 == 1)
    {
        SBytes[0] |= 0x80;
    }

    TArray<uint8> Out;
    Out.Append(RBytes);
    Out.Append(SBytes);
    return Out;
}

TSharedPtr<FRSY> FRSY::FromString(const FString& Input)
{
    TArray<FString> Parts;
    Input.ParseIntoArray(Parts, TEXT(":"), true);

    if (Parts.Num() != 3)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid RSY string: %s"), *Input);
        return nullptr;
    }

    FString RPart = FEthAbiBridge::HexToBigIntString(Parts[0].TrimStartAndEnd());
    FString SPart = FEthAbiBridge::HexToBigIntString(Parts[1].TrimStartAndEnd());
    FString VPart = FEthAbiBridge::HexToBigIntString(Parts[2].TrimStartAndEnd());

    int32 Y = VToYParity(BigIntMod2(VPart));
    return MakeShared<FRSY>(RPart, SPart, Y);
}

TSharedPtr<FRSY> FRSY::Unpack(const TArray<uint8>& RSY)
{
    if (RSY.Num() != 64)
    {
        UE_LOG(LogTemp, Error, TEXT("RSY must be exactly 64 bytes (%d given)"), RSY.Num());
        return nullptr;
    }

    TArray<uint8> RBytes;
    RBytes.Append(RSY.GetData(), 32);

    TArray<uint8> SBytes;
    SBytes.Append(RSY.GetData() + 32, 32);

    int32 YParity = (SBytes[0] & 0x80) ? 1 : 0;
    SBytes[0] &= 0x7F;

    FString RStr = FByteArrayUtils::BytesToHexString(RBytes);
    FString SStr = FByteArrayUtils::BytesToHexString(SBytes);

    return MakeShared<FRSY>(RStr, SStr, YParity);
}

TSharedPtr<FRSY> FRSY::UnpackFrom65(const TArray<uint8>& Sig65)
{
    if (Sig65.Num() != 65)
    {
        UE_LOG(LogTemp, Error, TEXT("Signature must be 65 bytes (%d given)"), Sig65.Num());
        return nullptr;
    }

    TArray<uint8> RBytes;
    RBytes.Append(Sig65.GetData(), 32);

    TArray<uint8> SBytes;
    SBytes.Append(Sig65.GetData() + 32, 32);

    uint8 V = Sig65[64];
    int32 Y = VToYParity(V);

    FString RStr = FByteArrayUtils::BytesToHexString(RBytes);
    FString SStr = FByteArrayUtils::BytesToHexString(SBytes);

    return MakeShared<FRSY>(RStr, SStr, Y);
}

