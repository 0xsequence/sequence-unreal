#include "SignatureHandler.h"

TSharedPtr<FRawSignature> SignatureHandler::EncodeSignature()
{
	return MakeShareable(new FRawSignature());
}

TSharedPtr<FConfigTopology> SignatureHandler::FillLeaves()
{
	return MakeShareable(new FConfigTopology());
}

TSharedPtr<FSignatureOfLeaf> SignatureHandler::SignatureForLeaf(const TSharedPtr<FConfigLeaf>& Leaf)
{
	return MakeShareable(new FSignatureOfLeaf());
}
