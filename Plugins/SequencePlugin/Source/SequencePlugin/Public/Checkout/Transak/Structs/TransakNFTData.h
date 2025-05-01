#pragma once

#include "CoreMinimal.h"
#include "TransakNftData.generated.h"

UENUM(BlueprintType)
enum class ENFTType : uint8
{
	ERC1155 UMETA(DisplayName = "ERC1155"),
	ERC721 UMETA(DisplayName = "ERC721")
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTransakNFTData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	FString ImageURL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	FString CollectionAddress;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	TArray<FString> TokenID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	TArray<float> Price;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	int Quantity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	ENFTType Type;
};