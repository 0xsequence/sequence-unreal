#include "EcosystemWallet/Primitives/Signatures/RawSignature.h"

#include "EcosystemWallet/Signatures/ChainedSignature.h"
#include "EcosystemWallet/Signatures/SignatureUtils.h"
#include "EthAbi/EthAbiBridge.h"
#include "Types/BigInt.h"
#include "Util/ByteArrayUtils.h"

TArray<uint8> FRawSignature::Encode(bool SkipCheckpointerData, bool SkipCheckpointerAddress)
{
    if (Suffix.Num() > 0)
    {
        FRawSignature Head;
        Head.NoChainId       = NoChainId;
        Head.CheckpointerData = CheckpointerData;
        Head.Configuration   = Configuration;
        Head.Erc6492        = nullptr;

        TArray<TSharedPtr<FRawSignature>> Chained;
        Chained.Add(MakeShared<FRawSignature>(Head));
        Chained.Append(Suffix);

        FChainedSignature ChainedSignature(Chained);
        TArray<uint8> ChainedEncoded = ChainedSignature.Encode();

        if (Erc6492 != nullptr)
        {
            return FErc6492::Wrap(
                Erc6492->Address,
                Erc6492->Data,
                ChainedEncoded
            );
        }

        return ChainedEncoded;
    }

    uint8 Flag = 0;
    if (NoChainId)
    {
        Flag |= 0x02;
    }

    int32 BytesForCheckpoint = 1;
    if (BytesForCheckpoint > 7)
    {
        UE_LOG(LogTemp, Error, TEXT("Checkpoint too large"));
        checkNoEntry();
    }
    
    Flag |= static_cast<uint8>(BytesForCheckpoint << 2);

    int32 BytesForThreshold = 1;
    BytesForThreshold = BytesForThreshold == 0 ? 1 : BytesForThreshold;
    if (BytesForThreshold > 2)
    {
        UE_LOG(LogTemp, Error, TEXT("Threshold too large"));
        checkNoEntry();
    }
    if (BytesForThreshold == 2)
    {
        Flag |= 0x20;
    }

    if (Configuration->Checkpointer.IsSet() && !SkipCheckpointerAddress)
    {
        Flag |= 0x40;
    }

    TArray<uint8> Output;
    Output.Add(Flag);

    if (Configuration->Checkpointer.IsSet() && !SkipCheckpointerAddress)
    {
        TArray<uint8> CheckpointerBytes = FByteArrayUtils::PadLeft(FByteArrayUtils::HexStringToBytes(Configuration->Checkpointer.GetValue()), 20);
        Output = FByteArrayUtils::ConcatBytes({Output, CheckpointerBytes});

        if (!SkipCheckpointerData)
        {
            int32 CheckpointerDataSize = CheckpointerData.Num();
            if (CheckpointerDataSize > 16777215)
            {
                UE_LOG(LogTemp, Error, TEXT("Checkpointer data too large"));
                checkNoEntry();
            }

            TArray<uint8> SizeBytes = FByteArrayUtils::ByteArrayFromNumber(CheckpointerDataSize, 3);
            Output = FByteArrayUtils::ConcatBytes({Output, SizeBytes, CheckpointerData});
        }
    }

    TArray<uint8> CheckpointBytes = Configuration->Checkpoint.Encode();
    Output = FByteArrayUtils::ConcatBytes({Output, CheckpointBytes});

    TArray<uint8> ThresholdBytes = Configuration->Threshold.Encode();
    Output = FByteArrayUtils::ConcatBytes({Output, ThresholdBytes});

    TArray<uint8> TopologyBytes = Configuration->Topology->Encode(NoChainId, CheckpointerData);
    Output = FByteArrayUtils::ConcatBytes({Output, TopologyBytes});

    if (Erc6492 != nullptr && Erc6492.IsValid())
    {
        return FErc6492::Wrap(Erc6492->Address, Erc6492->Data, Output);
    }

    return Output;
}

TSharedPtr<FRawSignature> FRawSignature::Decode(const TArray<uint8>& Erc6492Signature)
{
    TTuple<TArray<uint8>, TSharedPtr<FErc6492>> Decoded = FErc6492::Decode(Erc6492Signature);
    TArray<uint8> Signature = Decoded.Key;
    TSharedPtr<FErc6492> Erc6492 = Decoded.Value;

    if (Signature.Num() < 1)
    {
        UE_LOG(LogTemp, Error, TEXT("Signature is empty"));
    }

    int32 Index = 1;
    uint8 Flag = Signature[0];
    bool bNoChainId = (Flag & 0x02) == 0x02;

    FString CheckpointerAddress = "";
    TArray<uint8> CheckpointerData;

    // 2. Parse checkpointer data
    if ((Flag & 0x40) == 0x40)
    {
        CheckpointerAddress = FByteArrayUtils::BytesToHexString(Signature, Index, 20);
        Index += 20;

        int32 DataSize = FByteArrayUtils::BytesToInt(Signature, Index, 3);
        Index += 3;

        FByteArrayUtils::CopyBytes(Signature, Index, DataSize, CheckpointerData);
        Index += DataSize;
    }

    // 3. Parse checkpoint and threshold
    int32 CheckpointSize = (Flag & 0x1C) >> 2;
    FBigInt Checkpoint = FBigInt::FromHex(FByteArrayUtils::BytesToHexString(Signature, Index, CheckpointSize));
    Index += CheckpointSize;

    int32 ThresholdSize = ((Flag & 0x20) >> 5) + 1;
    FBigInt Threshold = FBigInt::FromHex(FByteArrayUtils::BytesToHexString(Signature, Index, ThresholdSize));
    Index += ThresholdSize;

    // 4. Handle chained signatures
    if ((Flag & 0x01) == 0x01)
    {
        TArray<TSharedPtr<FRawSignature>> SubSignatures;

        while (Index < Signature.Num())
        {
            int32 SubSize = FByteArrayUtils::BytesToInt(Signature, Index, 3);
            Index += 3;

            TArray<uint8> SubBytes = FByteArrayUtils::SliceBytes(Signature, Index, SubSize);
            Index += SubSize;

            TSharedPtr<FRawSignature> SubSig = FRawSignature::Decode(SubBytes);

            if (SubSig->CheckpointerData.Num() > 0)
            {
                UE_LOG(LogTemp, Error, TEXT("Chained subsignature has checkpointer data"));
            }

            SubSig->CheckpointerData.Empty();
            SubSignatures.Add(SubSig);
        }

        if (SubSignatures.Num() == 0)
        {
            UE_LOG(LogTemp, Error, TEXT("Chained signature has no subsignatures"));
        }

        TArray<TSharedPtr<FRawSignature>> NewSubSignatures;
        for (int32 i = 1; i < SubSignatures.Num(); i++)
        {
            NewSubSignatures.Add(SubSignatures[0]);
        }

        FRawSignature Result;
        Result.NoChainId = SubSignatures[0]->NoChainId;
        Result.CheckpointerData = SubSignatures[0]->CheckpointerData;
        Result.Configuration = SubSignatures[0]->Configuration;
        Result.Suffix = NewSubSignatures;
        Result.Erc6492 = Erc6492;
        
        return MakeShared<FRawSignature>(Result);
    }
    
    const TTuple<TArray<TSharedPtr<FConfigTopology>>, TArray<uint8>> ParsedBranch = FSignatureUtils::ParseBranch(FByteArrayUtils::SliceBytesFrom(Signature, Index));
    const TArray<TSharedPtr<FConfigTopology>> Nodes = ParsedBranch.Key;
    const TArray<uint8> Leftover = ParsedBranch.Value;
    
    if (Leftover.Num() != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Leftover bytes in signature"));
    }

    TSharedPtr<FConfigTopology> Topology = FSignatureUtils::FoldNodes(Nodes);

    FSeqConfig NewConfig = FSeqConfig(Threshold, Checkpoint, Topology);
    NewConfig.Checkpointer = CheckpointerAddress;

    FRawSignature Result;
    Result.NoChainId = bNoChainId;
    Result.CheckpointerData = CheckpointerData;
    Result.Configuration = MakeShared<FSeqConfig>(NewConfig);
    Result.Erc6492 = Erc6492;

    return MakeShared<FRawSignature>(Result);
}
