#pragma once
#include "Marketplace/Sardine/SardineCheckout.h"

#include "SardineGetNFTCheckoutTokenTest.generated.h"

class FSardineGetNFTCheckoutTokenTest;

UCLASS()
class USardineGetNFTCheckoutTokenTestHelper : public UObject
{
	GENERATED_BODY()
	
public:
	void RunTest();
	bool IsTestComplete() const { return bTestComplete; }

	FSardineGetNFTCheckoutTokenTest* ParentTest;

private:
	bool bTestComplete = false;
	USardineCheckout* Checkout = nullptr;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForGetNFTCheckoutToken, USardineGetNFTCheckoutTokenTestHelper*, TestHelper);
