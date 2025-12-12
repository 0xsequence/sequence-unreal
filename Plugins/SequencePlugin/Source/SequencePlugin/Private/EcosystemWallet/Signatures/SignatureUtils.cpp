#include "SignatureUtils.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigAnyAddressSubdigestLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigNestedLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigNodeLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSignedSapientSignerLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSignedSignerLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSignerLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSubdigestLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfSapientSignerLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfSignerLeafErc1271.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfSignerLeafEthSign.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfSignerLeafHash.h"
#include "Util/ByteArrayUtils.h"
#include "Util/ValueUtils.h"

TTuple<TArray<TSharedPtr<FConfigTopology>>, TArray<uint8>> FSignatureUtils::ParseBranch(const TArray<uint8>& Signature)
{
    TArray<TSharedPtr<FConfigTopology>> Leafs;
    TArray<uint8> Leftover;

    int32 Index = 0;
    const int32 Len = Signature.Num();

    while (Index < Len)
    {
        const uint8 FirstByte = Signature[Index++];
        const int32 Flag = (FirstByte & 0xF0) >> 4;

        switch (Flag)
        {
            case FConfigTopology::FlagSignatureHash:
            {
                int32 Weight = (FirstByte & 0x0F);
                if (Weight == 0)
                {
                    Weight = Signature[Index++];
                }

                const TArray<uint8> RsyBytes = FByteArrayUtils::SliceBytes(Signature, Index, 64);
                Index += 64;

                const TSharedPtr<FRSY> Rsy = FRSY::Unpack(RsyBytes);
                const TSharedPtr<FSignatureOfSignerLeafHash> HashSig = MakeShared<FSignatureOfSignerLeafHash>(FSignatureOfSignerLeafHash());
                HashSig->Signature = Rsy;

                FConfigSignedSignerLeaf Leaf = FConfigSignedSignerLeaf("", FValueUtils::Int32ToString(Weight), HashSig);
                Leafs.Add(MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigSignedSignerLeaf>(Leaf))));
                break;
            }

            case FConfigTopology::FlagAddress:
            {
                int32 Weight = (FirstByte & 0x0F);
                if (Weight == 0)
                {
                    Weight = Signature[Index++];
                }

                const FString AddressHex = FByteArrayUtils::BytesToHexString(Signature, Index, 20);
                Index += 20;

                FConfigSignerLeaf Leaf = FConfigSignerLeaf(AddressHex, FValueUtils::Int32ToString(Weight));

                Leafs.Add(MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigSignerLeaf>(Leaf))));
                break;
            }

            case FConfigTopology::FlagSignatureErc1271:
            {
                int32 Weight = (FirstByte & 0x03);
                if (Weight == 0)
                {
                    Weight = Signature[Index++];
                }

                const FString SignerHex = FByteArrayUtils::BytesToHexString(Signature, Index, 20);
                Index += 20;

                const int32 SizeSize = (FirstByte & 0x0C) >> 2;
                const int32 DataSize = FByteArrayUtils::BytesToInt(Signature, Index, SizeSize);
                Index += SizeSize;

                const TArray<uint8> Data = FByteArrayUtils::SliceBytes(Signature, Index, DataSize);
                Index += DataSize;

                TSharedPtr<FSignatureOfSignerLeafErc1271> Sig = MakeShared<FSignatureOfSignerLeafErc1271>(FSignatureOfSignerLeafErc1271(SignerHex, Data));

                FConfigSignedSignerLeaf Leaf = FConfigSignedSignerLeaf("", FValueUtils::Int32ToString(Weight), Sig);
                Leafs.Add(MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigSignedSignerLeaf>(Leaf))));
                break;
            }

            case FConfigTopology::FlagNode:
            {
                const TArray<uint8> NodeHash = FByteArrayUtils::SliceBytes(Signature, Index, 32);
                Index += 32;

                FConfigNodeLeaf Leaf = FConfigNodeLeaf(FByteArrayUtils::BytesToHexString(NodeHash));
                Leafs.Add(MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigNodeLeaf>(Leaf))));
                break;
            }

            case FConfigTopology::FlagBranch:
            {
                const int32 SizeSize = (FirstByte & 0x0F);
                const int32 Size = FByteArrayUtils::BytesToInt(Signature, Index, SizeSize);
                Index += SizeSize;

                const TArray<uint8> BranchBytes = FByteArrayUtils::SliceBytes(Signature, Index, Size);
                Index += Size;

                const TTuple<TArray<TSharedPtr<FConfigTopology>>, TArray<uint8>> Sub = ParseBranch(BranchBytes);
                if (Sub.Value.Num() > 0)
                {
                    UE_LOG(LogTemp, Error, TEXT("Leftover bytes in sub-branch"));
                }

                Leafs.Add(FoldNodes(Sub.Key));
                break;
            }

            case FConfigTopology::FlagSubdigest:
            {
                const TArray<uint8> Digest = FByteArrayUtils::SliceBytes(Signature, Index, 32);
                Index += 32;

                FConfigSubdigestLeaf Leaf = FConfigSubdigestLeaf(FByteArrayUtils::BytesToHexString(Digest));
                    
                Leafs.Add(MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigSubdigestLeaf>(Leaf))));
                break;
            }

            case FConfigTopology::FlagNested:
            {
                int32 ExternalWeight = (FirstByte & 0x0C) >> 2;
                if (ExternalWeight == 0)
                {
                    ExternalWeight = Signature[Index++];
                }

                int32 InternalThreshold = (FirstByte & 0x03);
                if (InternalThreshold == 0)
                {
                    InternalThreshold = FByteArrayUtils::BytesToInt(Signature, Index, 2);
                    Index += 2;
                }

                const int32 Size = FByteArrayUtils::BytesToInt(Signature, Index, 3);
                Index += 3;

                const TArray<uint8> NestedBytes = FByteArrayUtils::SliceBytes(Signature, Index, Size);
                Index += Size;

                const TTuple<TArray<TSharedPtr<FConfigTopology>>, TArray<uint8>> Sub = ParseBranch(NestedBytes);
                if (Sub.Value.Num() > 0)
                {
                    UE_LOG(LogTemp, Error, TEXT("Leftover bytes in nested tree"));
                }

                FConfigNestedLeaf Leaf = FConfigNestedLeaf(
                    FValueUtils::Int32ToString(ExternalWeight),
                    FValueUtils::Int32ToString(InternalThreshold),
                    FoldNodes(Sub.Key));

                Leafs.Add(MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigNestedLeaf>(Leaf))));
                break;
            }

            case FConfigTopology::FlagSignatureEthSign:
            {
                int32 Weight = (FirstByte & 0x0F);
                if (Weight == 0)
                {
                    Weight = Signature[Index++];
                }

                const TArray<uint8> RsyBytes = FByteArrayUtils::SliceBytes(Signature, Index, 64);
                Index += 64;

                const TSharedPtr<FRSY> Rsy = FRSY::Unpack(RsyBytes);

                const TSharedPtr<FSignatureOfSignerLeafEthSign> EthSignSig = MakeShared<FSignatureOfSignerLeafEthSign>(FSignatureOfSignerLeafEthSign(FValueUtils::Int32ToString(Weight), Rsy));
                FConfigSignedSignerLeaf Leaf = FConfigSignedSignerLeaf("", FValueUtils::Int32ToString(Weight), EthSignSig);

                Leafs.Add(MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigSignedSignerLeaf>(Leaf))));
                break;
            }

            case FConfigTopology::FlagSignatureAnyAddressSubdigest:
            {
                const TArray<uint8> Digest = FByteArrayUtils::SliceBytes(Signature, Index, 32);
                Index += 32;

                FConfigAnyAddressSubdigestLeaf Leaf = FConfigAnyAddressSubdigestLeaf(FByteArrayUtils::BytesToHexString(Digest));
                    
                Leafs.Add(MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigAnyAddressSubdigestLeaf>(Leaf))));
                break;
            }

            case FConfigTopology::FlagSignatureSapient:
            case FConfigTopology::FlagSignatureSapientCompact:
            {
                const bool bCompact = (Flag == FConfigTopology::FlagSignatureSapientCompact);

                int32 Weight = (FirstByte & 0x03);
                if (Weight == 0)
                {
                    Weight = Signature[Index++];
                }

                const FString AddressHex = FByteArrayUtils::BytesToHexString(Signature, Index, 20);
                Index += 20;

                const int32 SizeSize = (FirstByte & 0x0C) >> 2;
                const int32 DataSize = FByteArrayUtils::BytesToInt(Signature, Index, SizeSize);
                Index += SizeSize;

                const TArray<uint8> Data = FByteArrayUtils::SliceBytes(Signature, Index, DataSize);
                Index += DataSize;

                const ESapientSignatureType SigType = bCompact
                    ? ESapientSignatureType::SapientSignatureCompact
                    : ESapientSignatureType::SapientSignature;

                TSharedPtr<FSignatureOfSapientSignerLeaf> Sig = MakeShared<FSignatureOfSapientSignerLeaf>(FSignatureOfSapientSignerLeaf(SigType, AddressHex, Data));

                FConfigSignedSapientSignerLeaf Leaf = FConfigSignedSapientSignerLeaf("", FValueUtils::Int32ToString(Weight), "", Sig);

                Leafs.Add(MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigSignedSapientSignerLeaf>(Leaf))));
                break;
            }

            default:
            {
                UE_LOG(LogTemp, Error, TEXT("Invalid signature flag: 0x%X"), Flag);
                checkNoEntry();
                break;
            }
        }
    }

    if (Index < Len)
    {
        Leftover = FByteArrayUtils::SliceBytes(Signature, Index, Len - Index);
    }
    else
    {
        Leftover.Reset();
    }

    return TTuple<TArray<TSharedPtr<FConfigTopology>>, TArray<uint8>>(Leafs, Leftover);
}

TSharedPtr<FConfigTopology> FSignatureUtils::FoldNodes(const TArray<TSharedPtr<FConfigTopology>>& Nodes)
{
	if (Nodes.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Empty signature tree"));
		checkNoEntry();
	}

	if (Nodes.Num() == 1)
	{
		return Nodes[0];
	}

	TSharedPtr<FConfigTopology> Tree = Nodes[0];
	for (int32 i = 1; i < Nodes.Num(); ++i)
	{
		const TSharedPtr<FConfigNode> Node = MakeShared<FConfigNode>(FConfigNode(Tree, Nodes[i]));
		Tree = MakeShared<FConfigTopology>(FConfigTopology(Node));
	}
	
	return Tree;
}
