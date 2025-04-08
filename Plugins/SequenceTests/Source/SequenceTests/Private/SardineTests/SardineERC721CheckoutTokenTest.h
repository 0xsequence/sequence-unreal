#pragma once
#include "Marketplace/Sardine/SardineCheckout.h"

#include "SardineERC721CheckoutTokenTest.generated.h"

class FSardineERC721CheckoutTokenTest;

UCLASS()
class USardineERC721CheckoutTokenTestHelper : public UObject
{
	GENERATED_BODY()
	
public:
	void RunTest();
	bool IsTestComplete() const { return bTestComplete; }

	FSardineERC721CheckoutTokenTest* ParentTest;

private:
	bool bTestComplete = false;
	USardineCheckout* Checkout = nullptr;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForERC721CheckoutToken, USardineERC721CheckoutTokenTestHelper*, TestHelper);
