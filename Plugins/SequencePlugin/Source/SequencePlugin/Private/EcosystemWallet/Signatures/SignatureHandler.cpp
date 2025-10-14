#include "SignatureHandler.h"
#include "EcosystemWallet/Extensions/ExtensionsFactory.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigNestedLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSapientSignerLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSignedSapientSignerLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSignedSignerLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSignerLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/SapientSignature.h"

TSharedPtr<FRawSignature> FSignatureHandler::EncodeSignature(const TSharedPtr<FEnvelope>& Envelope, const FString& SessionsImageHash)
{
	const TFunction<TSharedPtr<FSignatureOfLeaf>(const TSharedPtr<FConfigLeaf>&)>& SignatureFor = [Envelope, SessionsImageHash](const TSharedPtr<FConfigLeaf>& Leaf)
	{
		return SignatureForLeaf(Envelope, Leaf, SessionsImageHash);
	};

	const TSharedPtr<FConfigTopology>& Topology = FillLeaves(Envelope->Config->Topology, SignatureFor);
	
	FRawSignature RawSignature;
	RawSignature.NoChainId = Envelope->ChainId.Value == "0";
	RawSignature.Configuration = MakeShared<FSeqConfig>(FSeqConfig(
		Envelope->Config->Threshold,
		Envelope->Config->Checkpoint,
		Topology));

	return MakeShared<FRawSignature>(RawSignature);
}

TSharedPtr<FConfigTopology> FSignatureHandler::FillLeaves(
	const TSharedPtr<FConfigTopology>& ConfigTopology,
	const TFunction<TSharedPtr<FSignatureOfLeaf>(const TSharedPtr<FConfigLeaf>&)>& SignatureFor)
{
	if (ConfigTopology->IsNode())
	{
		return MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigNode>(
			FillLeaves(ConfigTopology->Node->Left, SignatureFor),
			FillLeaves(ConfigTopology->Node->Right, SignatureFor))));
	}

	if (!ConfigTopology->IsLeaf())
	{
		return ConfigTopology;
	}

	FConfigLeaf* Leaf = ConfigTopology->Leaf.Get();

	if (Leaf->Type == EConfigLeafType::Signer)
	{
		auto* SignerLeaf = static_cast<FConfigSignerLeaf*>(Leaf);
		
		const TSharedPtr<FSignatureOfLeaf> Signature = SignatureFor(ConfigTopology->Leaf);
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
		
		const TSharedPtr<FSignatureOfLeaf> Signature = SignatureFor(ConfigTopology->Leaf);
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
			FillLeaves(NestedLeaf->Tree, SignatureFor))));
	}

	if (Leaf->Type == EConfigLeafType::Node ||
		Leaf->Type == EConfigLeafType::Subdigest ||
		Leaf->Type == EConfigLeafType::AnyAddressSubdigest)
	{
		return ConfigTopology;
	}
	
	return MakeShareable(new FConfigTopology());
}

TSharedPtr<FSignatureOfLeaf> FSignatureHandler::SignatureForLeaf(const TSharedPtr<FEnvelope>& Envelope, const TSharedPtr<FConfigLeaf>& Leaf, const FString& SessionsImageHash)
{
	if (Leaf->Type == EConfigLeafType::Signer)
	{
		auto* SignerLeaf = static_cast<FConfigSapientSignerLeaf*>(Leaf.Get());
		for (const TSharedPtr<FSignatureOfLeaf>& Sig : Envelope->Signatures)
		{
			if (Sig->Type == EConfigSignatureType::SignatureOfSigner)
			{
				if (SignerLeaf->Address.Equals(FExtensionsFactory::GetCurrent().Guard, ESearchCase::IgnoreCase))
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
