#pragma once

class SEQUENCEPLUGIN_API FConfigLeaf
{
public:
	virtual ~FConfigLeaf() = default;
	
	virtual TArray<uint8> Encode(const bool NoChainId, const TArray<uint8>& CheckpointerData)
	{
		return TArray<uint8>();
	}
	
	virtual TArray<uint8> HashConfiguration()
	{
		return TArray<uint8>();
	}
};
