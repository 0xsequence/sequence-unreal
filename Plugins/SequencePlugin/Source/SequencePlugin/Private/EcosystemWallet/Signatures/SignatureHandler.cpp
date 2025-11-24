#include "SignatureHandler.h"
#include "EcosystemWallet/Extensions/ExtensionsFactory.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigNestedLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSapientSignerLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSignedSapientSignerLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSignedSignerLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSignerLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/SapientSignature.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfSignerLeafHash.h"

TSharedPtr<FRawSignature> FSignatureHandler::EncodeSignature(
	const TSharedPtr<FEnvelope>& Envelope,
	const FString& SessionsImageHash)
{
	if (Envelope == nullptr || Envelope->Config == nullptr || Envelope->Config->Topology == nullptr ||
		!Envelope.IsValid() || !Envelope->Config.IsValid() || !Envelope->Config->Topology.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Envelope, config and/or topology is null"));
		return nullptr;
	}
	
	const TSharedPtr<FConfigTopology> Topology = FillLeaves(Envelope, Envelope->Config->Topology, SessionsImageHash);

	if (Topology == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Topology result is null"));
		return nullptr;
	}
	
	FRawSignature RawSignature;
	RawSignature.NoChainId = Envelope->ChainId.Value == "0";
	RawSignature.Configuration = MakeShared<FSeqConfig>(FSeqConfig(
		Envelope->Config->Threshold,
		Envelope->Config->Checkpoint,
		Topology));

	return MakeShared<FRawSignature>(RawSignature);
}

TSharedPtr<FConfigTopology> FSignatureHandler::FillLeaves(
	const TSharedPtr<FEnvelope>& Envelope,
	const TSharedPtr<FConfigTopology>& ConfigTopology,
	const FString& SessionsImageHash)
{
	if (ConfigTopology->IsNode())
	{
		return MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigNode>(
			FillLeaves(Envelope, ConfigTopology->Node->Left, SessionsImageHash),
			FillLeaves(Envelope, ConfigTopology->Node->Right, SessionsImageHash))));
	}

	if (!ConfigTopology->IsLeaf())
	{
		return ConfigTopology;
	}

	FConfigLeaf* Leaf = ConfigTopology->Leaf.Get();

	if (Leaf->Type == EConfigLeafType::Signer)
	{
		auto* SignerLeaf = static_cast<FConfigSignerLeaf*>(Leaf);
		
		const TSharedPtr<FSignatureOfLeaf> Signature = SignatureForLeaf(Envelope, ConfigTopology->Leaf);
		if (Signature == nullptr)
		{
			return ConfigTopology;
		}

		const TSharedPtr<FConfigSignedSignerLeaf> NewLeaf = MakeShared<FConfigSignedSignerLeaf>(SignerLeaf->Address, SignerLeaf->Weight, Signature);
		return MakeShared<FConfigTopology>(FConfigTopology(NewLeaf));
	}

	if (Leaf->Type == EConfigLeafType::SapientSigner)
	{
		auto* SapientLeaf = static_cast<FConfigSapientSignerLeaf*>(Leaf);
		
		const TSharedPtr<FSignatureOfLeaf> Signature = SignatureForLeaf(Envelope, ConfigTopology->Leaf);
		if (Signature == nullptr)
		{
			return ConfigTopology;
		}

		const TSharedPtr<FConfigSignedSapientSignerLeaf> NewLeaf = MakeShared<FConfigSignedSapientSignerLeaf>(SapientLeaf->Address, SapientLeaf->Weight, SapientLeaf->ImageHash, Signature);
		return MakeShared<FConfigTopology>(FConfigTopology(NewLeaf));
	}

	if (Leaf->Type == EConfigLeafType::Nested)
	{
		auto* NestedLeaf = static_cast<FConfigNestedLeaf*>(Leaf);
		return MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigNestedLeaf>(
			NestedLeaf->Weight,
			NestedLeaf->Threshold,
			FillLeaves(Envelope, NestedLeaf->Tree, SessionsImageHash))));
	}

	if (Leaf->Type == EConfigLeafType::Node ||
		Leaf->Type == EConfigLeafType::Subdigest ||
		Leaf->Type == EConfigLeafType::AnyAddressSubdigest)
	{
		return ConfigTopology;
	}
	
	return MakeShareable(new FConfigTopology());
}

TSharedPtr<FSignatureOfLeaf> FSignatureHandler::SignatureForLeaf(const TSharedPtr<FEnvelope>& Envelope, const TSharedPtr<FConfigLeaf>& Leaf)
{
	if (Leaf->Type == EConfigLeafType::Signer)
	{
		auto* SignerLeaf = static_cast<FConfigSapientSignerLeaf*>(Leaf.Get());
		for (const TSharedPtr<FSignatureOfLeaf>& Sig : Envelope->Signatures)
		{
			if (Sig->Type == EConfigSignatureType::SignatureOfSigner)
			{
				auto* Signature = static_cast<FSignatureOfSignerLeafHash*>(Sig.Get());
				if (SignerLeaf->Address.Equals(Signature->Address, ESearchCase::IgnoreCase))
				{
					return Sig;
				}
			}
		}
	}
	else if (Leaf->Type == EConfigLeafType::SapientSigner)
	{
		auto* SapientSignerLeaf = static_cast<FConfigSapientSignerLeaf*>(Leaf.Get());
		for (const TSharedPtr<FSignatureOfLeaf>& Sig : Envelope->Signatures)
		{
			if (Sig->Type == EConfigSignatureType::SignatureOfSapientSigner)
			{
				auto* SapientSignature = static_cast<FSignatureOfSapientSignerLeaf*>(Sig.Get());
				if (SapientSignature->Address.Equals(SapientSignerLeaf->Address, ESearchCase::IgnoreCase))
				{
					return Sig;
				}
			}
		}
	}
	
	return nullptr;
}
