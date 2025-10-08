#pragma once

enum ESessionsLeafType
{
	SessionsNode,
	IdentitySigner,
	ImplicitBlacklist,
	Permissions
};


class SEQUENCEPLUGIN_API FSessionsLeaf
{
public:
	virtual ~FSessionsLeaf() = default;
	
	virtual TArray<uint8> Encode()
	{
		return TArray<uint8>();
	}
	
	virtual TArray<uint8> EncodeForHash()
	{
		return TArray<uint8>();
	}

	ESessionsLeafType Type;
};
