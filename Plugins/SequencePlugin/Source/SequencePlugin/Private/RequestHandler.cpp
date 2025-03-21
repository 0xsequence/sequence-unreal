// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "RequestHandler.h"
#include "Modules/ModuleManager.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Util/Async.h"
#include "IImageWrapperModule.h"
#include "Http.h"
#include "IImageWrapper.h"
#include "Types/BinaryData.h"
#include "Util/HexUtility.h"
#include "Util/Log.h"
#include "TextureResource.h"

URequestHandler* URequestHandler::PrepareRequest()
{
	Request = FHttpModule::Get().CreateRequest();
	Request->SetTimeout(30);
	return this;
}

void URequestHandler::SetUrl(const FString Url) const
{
	Request->SetURL(Url);
}

void URequestHandler::SetVerb(FString Verb) const
{
	Request->SetVerb(Verb);
}

void URequestHandler::AddHeader(const FString Name, const FString Value) const
{
	Request->AppendToHeader(Name, Value);
}

void URequestHandler::SetContentAsString(const FString Content) const
{
	Request->SetContentAsString(Content);
}

URequestHandler* URequestHandler::WithUrl(const FString Url)
{
	SetUrl(Url);
	return this;
}

URequestHandler* URequestHandler::WithVerb(const FString Verb)
{
	SetVerb(Verb);
	return this;
}

URequestHandler* URequestHandler::WithHeader(const FString Name, const FString Value)
{
	AddHeader(Name, Value);
	return this;
}

URequestHandler* URequestHandler::WithContentAsString(const FString Content)
{
	SetContentAsString(Content);
	return this;
}

FHttpRequestCompleteDelegate& URequestHandler::Process() const
{
	Request->ProcessRequest();
	return Request->OnProcessRequestComplete();
}


void URequestHandler::ProcessAndThen(UResponseSignatureValidator& Validator, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure, bool bUseValidator) const
{
	if (bUseValidator && Validator.HasFoundTamperedResponse())
	{
		UE_LOG(LogTemp, Error, TEXT("Validator is null!"));
		OnFailure(FSequenceError(RequestFail, "Validator is null."));
		return;
	}

	Process().BindLambda([&Validator, bUseValidator, OnSuccess, OnFailure](FHttpRequestPtr Req, const FHttpResponsePtr& Response, const bool bWasSuccessful)
		{
			if (bWasSuccessful)
			{
				if (!bUseValidator || Validator.ValidateResponseSignature(Response))
				{
					UE_LOG(LogTemp, Log, TEXT("Valid Signature or Validator skipped"));
					OnSuccess(Response->GetContentAsString());
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Invalid Signature"));
					OnFailure(FSequenceError(RequestFail, "Invalid response Signature"));
				}
			}
			else
			{
				if (Response.IsValid())
				{
					OnFailure(FSequenceError(RequestFail, "Request is invalid: " + Response->GetContentAsString()));
				}
				else
				{
					OnFailure(FSequenceError(RequestFail, "Request failed: No response received!"));
				}
			}
		});
}

void URequestHandler::ProcessAndThen(const TSuccessCallback<FHttpResponsePtr>& OnSuccess,
	const FFailureCallback& OnFailure) const
{
	Process().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Req, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		const int32 Code = Response->GetResponseCode();
		if (bWasSuccessful && Code >= 200 && Code < 300)
		{
			OnSuccess(Response);
			return;
		}

		if (!Response.IsValid())
		{
			OnFailure(FSequenceError(RequestFail, Response, "The Request is invalid!"));
			return;
		}

		const FString Content = Response->GetContentAsString();
		OnFailure(FSequenceError(RequestFail, Response, "Request failed: " + Content));
	});
}


void URequestHandler::ProcessAndThen(const TSuccessCallback<UTexture2D*>& OnSuccess, const FFailureCallback OnFailure) const
{
	Process().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		auto content = Req->GetContent();
		FString str = "";
		for (auto i : content)
		{
			str += UTF8ToString(FUnsizedData{ MakeArray(&i, 1) });
		}
		auto headers = Req->GetAllHeaders();
		FString headers_str = "";
		for (auto header : headers) { headers_str += "\n" + header; }

		if (bWasSuccessful)
		{
			TArray<uint8> img_data = Response->GetContent();
			//now we must process the image
			int32 width = 0, height = 0;
			UTexture2D* img = nullptr;
			EPixelFormat pxl_format = PF_B8G8R8A8;
			EImageFormat img_format = EImageFormat::PNG;//the default format for qr codes is png

			IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
			TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(img_format);

			//we might not be able to have this in here and instead may need to be in an outer function to work
			if (ImageWrapper && ImageWrapper.Get()->SetCompressed(img_data.GetData(), img_data.Num()))
			{
				TArray64<uint8>  Uncompressed;
				if (ImageWrapper.Get()->GetRaw(Uncompressed))
				{
					width = ImageWrapper.Get()->GetWidth();
					height = ImageWrapper.Get()->GetHeight();

					img = UTexture2D::CreateTransient(width, height, pxl_format);
					if (!img)
					{
						void* TextureData = img->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
						FMemory::Memcpy(TextureData, Uncompressed.GetData(), Uncompressed.Num());
						img->GetPlatformData()->Mips[0].BulkData.Unlock();
						img->UpdateResource();
						return OnSuccess(img);//return the properly built image data!
					}//valid image pointer
				}//able to get raw image data
			}//image wrapper is setup and ready to go
		}//if was successful
		else
		{
			if (!Response.IsValid())
			{
				OnFailure(FSequenceError(RequestFail, "The Request is invalid!"));
			} else
			{
				OnFailure(FSequenceError(RequestFail, "Request failed: " + Response->GetContentAsString()));
			}
		}//if wasn't successful
		//catch all error case!
		OnFailure(FSequenceError(RequestFail, "Failed to build QR Image data"));
	});//lambda
}
