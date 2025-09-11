#pragma once

#include "CoreMinimal.h"
#include "WebBrowser.h"
#include "Blueprint/UserWidget.h"
#include "BrowserOverlay.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UBrowserOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	void LoadUrl(const FString& Url);

private:
	UPROPERTY()
	UWebBrowser* Browser;

	UFUNCTION()
	void HandleConsoleMessage(const FString& Message, const FString& Source, int32 Line);

	UFUNCTION()
	void HandleUrlChanged(const FText& NewUrl);
};
