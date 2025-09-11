#include "BrowserOverlay.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "WebBrowserWidget/Public/WebBrowser.h"

bool UBrowserOverlay::Initialize()
{
	Super::Initialize();
	
	// Root canvas (full screen)
	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = RootCanvas;

	// --- Background image ---
	UImage* Background = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());

	// Make it black with some transparency
	FLinearColor DimColor = FLinearColor(0, 0, 0, 1);
	Background->SetColorAndOpacity(DimColor);

	UCanvasPanelSlot* BGSlot = RootCanvas->AddChildToCanvas(Background);
	BGSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));  // full screen
	BGSlot->SetOffsets(FMargin(0.f));

	// --- Web Browser ---
	UWebBrowser* Browser = WidgetTree->ConstructWidget<UWebBrowser>(UWebBrowser::StaticClass());
	this->Browser = Browser;
	Browser->OnConsoleMessage.AddDynamic(this, &UBrowserOverlay::HandleConsoleMessage);
	Browser->OnUrlChanged.AddDynamic(this, &UBrowserOverlay::HandleUrlChanged);

	UCanvasPanelSlot* BrowserSlot = RootCanvas->AddChildToCanvas(Browser);
	BrowserSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f)); // fill screen
	BrowserSlot->SetOffsets(FMargin(50.f)); // optional margin inside background

	return true;
}

void UBrowserOverlay::LoadUrl(const FString& Url)
{
	UE_LOG(LogTemp, Log, TEXT("Loading URL: %s"), *Url);
	this->Browser->LoadURL(TEXT("https://v3.sequence-dev.app/"));
}

void UBrowserOverlay::HandleConsoleMessage(const FString& Message, const FString& Source, int32 Line)
{
	UE_LOG(LogTemp, Warning, TEXT("WebBrowser Console: %s (Source: %s, Line: %d)"),
		*Message, *Source, Line);
}

void UBrowserOverlay::HandleUrlChanged(const FText& NewUrl)
{
	FString UrlStr = NewUrl.ToString();
	UE_LOG(LogTemp, Warning, TEXT("WebBrowser navigated to: %s"), *UrlStr);

	if (UrlStr.Contains(TEXT("auth/callback")))
	{
		UE_LOG(LogTemp, Warning, TEXT("OAuth callback detected: %s"), *UrlStr);
	}
}
