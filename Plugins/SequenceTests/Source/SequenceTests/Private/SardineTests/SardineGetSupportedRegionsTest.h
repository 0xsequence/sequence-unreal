#pragma once
#include "Marketplace/Sardine/SardineCheckout.h"

#include "SardineGetSupportedRegionsTest.generated.h"

class FSardineGetSupportedRegionsTest;

UCLASS()
class USardineGetSupportedRegionsTestHelper : public UObject
{
	GENERATED_BODY()
	
public:
	void RunTest();
	bool IsTestComplete() const { return bTestComplete; }

	FSardineGetSupportedRegionsTest* ParentTest;

private:
	bool bTestComplete = false;
	USardineCheckout* Checkout = nullptr;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForGetSupportedRegionsToComplete, USardineGetSupportedRegionsTestHelper*, TestHelper);
