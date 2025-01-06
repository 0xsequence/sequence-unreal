#include "Actions/RequestTextureAsyncAction.h"

#include "ObjectHandler.h"
#include "Util/Log.h"

URequestTextureAsyncAction* URequestTextureAsyncAction::RequestTextureAsync(const FString& Url)
{
	URequestTextureAsyncAction* Action = NewObject<URequestTextureAsyncAction>();
	Action->Url = Url;
	return Action;
}

void URequestTextureAsyncAction::Activate()
{
	UObjectHandler* ObjectHandler = NewObject<UObjectHandler>();
	ObjectHandler->Setup(false);

	ObjectHandler->FOnDoneImageProcessingDelegate.BindLambda([this, ObjectHandler]()
	{
		TMap<FString, UTexture2D*> Images = ObjectHandler->GetProcessedImages();
		if (Images.Contains(this->Url))
		{
			SEQ_LOG_EDITOR(Log, TEXT("Successfully downloaded texture."));
			this->OnSuccess.Broadcast(*Images.Find(this->Url));
		}
		else
		{
			SEQ_LOG_EDITOR(Error, TEXT("Failed to download texture."));
			this->OnFailure.Broadcast(nullptr);
		}
	});

	ObjectHandler->RequestImage(this->Url);
}
