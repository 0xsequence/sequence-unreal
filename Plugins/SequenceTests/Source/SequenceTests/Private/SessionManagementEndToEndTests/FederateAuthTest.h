// FederateAuthTest.h
#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Sequence/SequenceWallet.h"
#include "SequencePlugin/Public/SequenceAuthenticator.h"
#include "FederateAuthTest.generated.h"

class FWaaSFederatedAuthTests;

UCLASS()
class UWaaSFederatedAuthTestsHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnFederateSuccess();

	UFUNCTION()
	void OnFederateFailure(const FString& Error);

	UFUNCTION()
	void OnAuthSuccess();

	UFUNCTION()
	void OnAuthFailure(const FString& Error);

	void RunTest();
	bool IsTestComplete() const { return bTestComplete; }

	FWaaSFederatedAuthTests* ParentTest;

private:
	bool bTestComplete = false;
	USequenceAuthenticator* Authenticator = nullptr;
	FString Username;
	FString Email;
	FString Password;
};

// Remove the FWaaSFederatedAuthTests class declaration from here

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForFederationToComplete, UWaaSFederatedAuthTestsHelper*, TestHelper);