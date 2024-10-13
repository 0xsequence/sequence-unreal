// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SequenceWalletQrCode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTextureChanged);

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceWalletQrCode : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Sequence Events")
	FOnTextureChanged OnTextureChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void LoadQrImage();

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void SetGeneratorUrl(FString GeneratorUrl);

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void SetFormat(FString Format);

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void SetTextureSize(int32 Size);

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	UTexture2D* GetTexture();

private:
	FString GeneratorUrl = "https://api.sequence.app/qr/";
	FString Format = "{0}";
	uint Size = 256;
	UTexture2D* Texture;

	FString ConstructUrl(FString WalletAddress) const;
	static FString EncodeBase64Url(const FString& Data);
};
