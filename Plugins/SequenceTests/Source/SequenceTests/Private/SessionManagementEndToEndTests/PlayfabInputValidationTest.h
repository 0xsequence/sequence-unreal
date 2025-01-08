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
	void SetIsLoginTest(bool bIsLogin) { bIsLoginTest = true; }
	bool IsLoginTest() const { return bIsLoginTest; }
	void SetIsNewAccountTest(bool bIsNewAccount) { bIsNewAccountTest = true; }
	bool IsNewAccountTest() const { return bIsNewAccountTest; }
	void SetIsFederationTest(bool bIsFederation) { bIsFederationTest = true; }
	bool IsFederationTest() const { return bIsFederationTest; }
	void SetIsFederationLoginTest(bool bIsFederationLogin) { bIsFederationLoginTest = true; }
	bool IsFederationLoginTest() const { return bIsFederationLoginTest; }

	void RunNewAccountValidationTests();
	void RunFederationLoginValidationTests();

	class FAutomationTestBase* ParentTest;

private:
	bool bTestComplete = false;
	bool bTestPassed = false;
	bool bAllTestsComplete = false;
	FString LastError;
	USequenceAuthenticator* Authenticator = nullptr;
	bool bIsLoginTest = false;
	bool bIsNewAccountTest = false;
	bool bIsFederationTest = false;
	bool bIsFederationLoginTest = false;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForValidationTestComplete, UPlayFabInputValidationTestHelper*, TestHelper);
