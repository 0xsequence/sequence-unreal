#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "SequencePlugin/Public/SequenceAuthenticator.h"
#include "PlayfabInputValidationTest.generated.h"

UCLASS()
class UPlayFabInputValidationTestHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnAuthSuccess();

	UFUNCTION()
	void OnAuthFailure(const FString& Error);

	UFUNCTION()
	void OnFederateSuccess();

	UFUNCTION()
	void OnFederateFailure(const FString& Error);

	void RunLoginValidationTests();
	void RunFederationValidationTests();
	bool IsTestComplete() const { return bTestComplete; }
	bool GetTestPassed() const { return bTestPassed; }
	bool AreAllTestsComplete() const { return bAllTestsComplete; }
	FString GetLastError() const { return LastError; }
	void SetIsLoginTest(bool bIsLogin) { bIsLoginTest = bIsLogin; }
	bool IsLoginTest() const { return bIsLoginTest; }

	class FAutomationTestBase* ParentTest;

private:
	bool bTestComplete = false;
	bool bTestPassed = false;
	bool bAllTestsComplete = false;
	FString LastError;
	USequenceAuthenticator* Authenticator = nullptr;
	bool bIsLoginTest = false;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForValidationTestComplete, UPlayFabInputValidationTestHelper*, TestHelper);
