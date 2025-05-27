#pragma once

#include "CoreMinimal.h"
#include "Util/SequenceSupport.h"
#include "ERC721SaleDetails.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FERC721SaleDetails
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - ERC721 Sale")
	int32 SupplyCap;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - ERC721 Sale")
	FString Cost;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - ERC721 Sale")
	FString PaymentToken;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - ERC721 Sale")
	int32 StartTime;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - ERC721 Sale")
	int32 EndTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - ERC721 Sale")
	FString MerkleRoot;

	/**
	 * Fill this struct from an encoded JSON array.
	 * @param EncodedFunctionResult The encoded data you receive when reading the chain using SequenceWallet::Call();
	 */
	void DecodeFromFunctionReturn(const FString& EncodedFunctionResult)
	{
		const FString Decoded = USequenceSupport::DecodeFunctionResult(AbiJson, EncodedFunctionResult);
		
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Decoded);
		if (TSharedPtr<FJsonValue> JsonValue; FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
		{
			const TArray<TSharedPtr<FJsonValue>>* OuterArray;
			if (JsonValue->TryGetArray(OuterArray) && OuterArray->Num() > 0)
			{
				const TArray<TSharedPtr<FJsonValue>>* InnerArray;
				if ((*OuterArray)[0]->TryGetArray(InnerArray) && InnerArray->Num() >= 5)
				{
					SupplyCap = static_cast<int32>((*InnerArray)[0]->AsNumber());
					Cost = (*InnerArray)[1]->AsString();
					PaymentToken = (*InnerArray)[2]->AsString();
					StartTime = static_cast<int32>((*InnerArray)[3]->AsNumber());
					EndTime = static_cast<int32>((*InnerArray)[4]->AsNumber());
					MerkleRoot = (*InnerArray)[5]->AsString();
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Tuple from struct SaleDetails is missing or has 0 values in it: %s"), *Decoded);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Output array is missing or empty: %s"), *Decoded);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON string: %s"), *Decoded);
		}
	}

private:
	static inline FString AbiJson = R"({"type": "function","name": "saleDetails","inputs": [],"outputs": [{"name": "","type": "tuple","internalType": "struct IERC721SaleFunctions.SaleDetails","components": [{"name": "supplyCap","type": "uint256","internalType": "uint256"},{"name": "cost","type": "uint256","internalType": "uint256"},{"name": "paymentToken","type": "address","internalType": "address"},{"name": "startTime","type": "uint64","internalType": "uint64"},{"name": "endTime","type": "uint64","internalType": "uint64"},{"name": "merkleRoot","type": "bytes32","internalType": "bytes32"}]}],"stateMutability": "view"})";
};
