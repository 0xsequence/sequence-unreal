#include "BrowserRedirectHandler.h"
#include "EcosystemWallet/Authentication/Browser/BrowserOverlay.h"
#include "Kismet/GameplayStatics.h"

void FBrowserRedirectHandler::WaitForResponseImpl(const FString& FullUrl, TSuccessCallback<FString> OnSuccess,
                                                  FFailureCallback OnFailure)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(nullptr, 0);
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("PlayerController is null"));
		return;
	}
    
	UBrowserOverlay* Overlay = CreateWidget<UBrowserOverlay>(PlayerController, UBrowserOverlay::StaticClass());
	Overlay->AddToViewport(10000);
	Overlay->LoadUrl(FullUrl);
}
