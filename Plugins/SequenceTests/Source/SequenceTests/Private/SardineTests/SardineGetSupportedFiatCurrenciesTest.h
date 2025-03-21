#pragma once
#include "Marketplace/Sardine/SardineCheckout.h"

#include "SardineGetSupportedFiatCurrenciesTest.generated.h"

class FSardineGetSupportedFiatCurrenciesTest;

UCLASS()
class USardineGetSupportedFiatCurrenciesTestHelper : public UObject
{
	GENERATED_BODY()
	
public:
	void RunTest();
	bool IsTestComplete() const { return bTestComplete; }

	FSardineGetSupportedFiatCurrenciesTest* ParentTest;

private:
	bool bTestComplete = false;
	USardineCheckout* Checkout = nullptr;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForGetSupportedFiatCurrenciesToComplete, USardineGetSupportedFiatCurrenciesTestHelper*, TestHelper);
