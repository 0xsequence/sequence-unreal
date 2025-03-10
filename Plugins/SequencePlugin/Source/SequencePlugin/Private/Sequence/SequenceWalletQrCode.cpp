// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "../../Public/Sequence/SequenceWalletQrCode.h"

#include "ObjectHandler.h"
#include "Misc/Base64.h"
#include "Util/Log.h"
#include "Sequence/SequenceWallet.h"

void USequenceWalletQrCode::LoadQrImage()
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (!WalletOptional.IsSet() || !WalletOptional.GetValue())
	{
		SEQ_LOG(Display, TEXT("Wallet not defined."));
		return;
	}

	const USequenceWallet* Wallet = WalletOptional.GetValue();
	UObjectHandler* ObjectHandler = NewObject<UObjectHandler>();
	ObjectHandler->SetupCustomFormat(true, EImageFormat::GrayscaleJPEG);

	const FString WalletAddress = Wallet->GetWalletAddress();
	FString QrCodeUrl = this->ConstructUrl(WalletAddress);
	ObjectHandler->FOnDoneImageProcessingDelegate.BindLambda([this, ObjectHandler, QrCodeUrl]()
	{
		TMap<FString, UTexture2D*> Images = ObjectHandler->GetProcessedImages();
		if (Images.Contains(QrCodeUrl))
		{
			SEQ_LOG_EDITOR(Error, TEXT("Successfully downloaded QR Code."));
			this->Texture = *Images.Find(QrCodeUrl);
			this->OnTextureChanged.Broadcast();
		}
		else
		{
			SEQ_LOG_EDITOR(Error, TEXT("Failed to fetch QR Code"));
		}
	});

	ObjectHandler->RequestImage(QrCodeUrl);
}

void USequenceWalletQrCode::SetGeneratorUrl(FString Url)
{
	this->GeneratorUrl = Url;
}


void USequenceWalletQrCode::SetFormat(FString Format)
{
	this->UrlFormat = Format;
}

void USequenceWalletQrCode::SetTextureSize(int32 Size)
{
	this->TextureSize = Size;
}

UTexture2D* USequenceWalletQrCode::GetTexture()
{
	return this->Texture;
}

FString USequenceWalletQrCode::ConstructUrl(FString WalletAddress) const
{
	FString Size = "/";
	Size.AppendInt(this->TextureSize);  

	const FString Data = FString::Format(*this->UrlFormat, { WalletAddress });  
	return this->GeneratorUrl + EncodeBase64Url(Data) + Size;
}

FString USequenceWalletQrCode::EncodeBase64Url(const FString& Data)
{
	FString ret = FBase64::Encode(Data);
	const FString srch_plus = TEXT("+");
	const FString rep_plus = TEXT("-");
	const FString srch_slash = TEXT("/");
	const FString rep_slash = TEXT("_");

	const TCHAR* srch_ptr_plus = *srch_plus;
	const TCHAR* rep_ptr_plus = *rep_plus;
	const TCHAR* srch_ptr_slash = *srch_slash;
	const TCHAR* rep_ptr_slash = *rep_slash;

	ret.ReplaceInline(srch_ptr_plus, rep_ptr_plus, ESearchCase::IgnoreCase);
	ret.ReplaceInline(srch_ptr_slash, rep_ptr_slash, ESearchCase::IgnoreCase);
	return ret;
}
