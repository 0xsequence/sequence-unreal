#pragma once

class SEQUENCEPLUGIN_API FErc6492
{
public:
	FErc6492() : Address(TEXT("")), Data(TArray<uint8>()) { }
	explicit FErc6492(const FString& Address, const TArray<uint8>& Data) : Address(Address), Data(Data) { }

	static TArray<uint8> CreateDeployData(const FString& Stage1, const FString& DeployHash);

	static TArray<uint8> Wrap(const FString& To, const TArray<uint8>& Data, const TArray<uint8>& Signature);
	
	static TTuple<TArray<uint8>, TSharedPtr<FErc6492>> Decode(const TArray<uint8>& Signature);
	
	FString Address;
	TArray<uint8> Data;

private:
	static inline const FString MagicBytesHex = TEXT("0x6492649264926492649264926492649264926492649264926492649264926492");
};
