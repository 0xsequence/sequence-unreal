#include "EcosystemWallet/Primitives/Signatures/Erc6492.h"
#include "EthAbi/EthAbiBridge.h"
#include "Util/ByteArrayUtils.h"

TArray<uint8> FErc6492::CreateDeployData(const FString& Stage1, const FString& DeployHash)
{
    const FString Args = FString::Printf(TEXT("[\"%s\",\"%s\"]"), *Stage1, *DeployHash);
    const FString DataAsHex = FEthAbiBridge::EncodeFunctionCall("deploy(address,bytes32)", Args);
    return FByteArrayUtils::HexStringToBytes(DataAsHex);
}

TArray<uint8> FErc6492::Wrap(const FString& To, const TArray<uint8>& Data, const TArray<uint8>& Signature)
{
    const FString DataHex = FByteArrayUtils::BytesToHexString(Data);
    const FString SignatureHex = FByteArrayUtils::BytesToHexString(Signature);
    const FString Args = FString::Printf(TEXT("[\"%s\",\"%s\",\"%s\"]"), *To, *DataHex, *SignatureHex);
	return FEthAbiBridge::EncodeAbiPacked(Args);
}

TTuple<TArray<uint8>, TSharedPtr<FErc6492>> FErc6492::Decode(const TArray<uint8>& Signature)
{
    TArray<uint8> MagicBytes = FByteArrayUtils::HexStringToBytes(MagicBytesHex);
    int32 MagicLength = MagicBytes.Num();

    if (Signature.Num() >= MagicLength)
    {
        TArray<uint8> SignatureEnd;
        SignatureEnd.Append(&Signature[Signature.Num() - MagicLength], MagicLength);

        FString SignatureEndHex = FByteArrayUtils::BytesToHexString(SignatureEnd);

        if (SignatureEndHex.Equals(MagicBytesHex, ESearchCase::IgnoreCase))
        {
            TArray<uint8> Raw;
            Raw.Append(Signature.GetData(), Signature.Num() - MagicLength);

            const FString Types = FString::Printf(TEXT("[\"address\",\"bytes\",\"bytes\"]"));
            TArray<FString> Decoded = FEthAbiBridge::DecodeAbiJson(Raw, Types);

            FString To;
            TArray<uint8> Data;
            TArray<uint8> UnwrappedSignature;

            if (Decoded.Num() >= 3)
            {
                To = Decoded[0];
                Data = FByteArrayUtils::HexStringToBytes(Decoded[1]);
                UnwrappedSignature = FByteArrayUtils::HexStringToBytes(Decoded[2]);
            }
            else
            {
                return MakeTuple(Signature, MakeShared<FErc6492>());
            }

            TSharedPtr<FErc6492> Result = MakeShared<FErc6492>(To, Data);
            return MakeTuple(UnwrappedSignature, Result);
        }
    }

    return MakeTuple(Signature, MakeShared<FErc6492>());
}
