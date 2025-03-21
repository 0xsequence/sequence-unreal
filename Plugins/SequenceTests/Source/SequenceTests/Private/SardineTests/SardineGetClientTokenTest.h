#pragma once
#include "Marketplace/Sardine/SardineCheckout.h"

#include "SardineGetClientTokenTest.generated.h"

class FSardineGetClientTokenTest;

UCLASS()
class USardineGetClientTokenTestHelper : public UObject
{
	GENERATED_BODY()
	
public:
	void RunTest();
	bool IsTestComplete() const { return bTestComplete; }

	FSardineGetClientTokenTest* ParentTest;

private:
	bool bTestComplete = false;
	USardineCheckout* Checkout = nullptr;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForGetClientTokenToComplete, USardineGetClientTokenTestHelper*, TestHelper);
