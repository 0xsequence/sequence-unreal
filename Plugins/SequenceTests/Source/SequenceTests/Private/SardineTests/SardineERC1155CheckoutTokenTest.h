#pragma once
#include "Marketplace/Sardine/SardineCheckout.h"

#include "SardineERC1155CheckoutTokenTest.generated.h"

class FSardineERC1155CheckoutTokenTest;

UCLASS()
class USardineERC1155CheckoutTokenTestHelper : public UObject
{
	GENERATED_BODY()
	
public:
	void RunTest();
	bool IsTestComplete() const { return bTestComplete; }

	FSardineERC1155CheckoutTokenTest* ParentTest;

private:
	bool bTestComplete = false;
	USardineCheckout* Checkout = nullptr;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForERC1155CheckoutToken, USardineERC1155CheckoutTokenTestHelper*, TestHelper);
