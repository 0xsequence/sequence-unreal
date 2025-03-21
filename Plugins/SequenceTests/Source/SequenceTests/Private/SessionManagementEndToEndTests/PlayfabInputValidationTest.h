#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Sequence/SequenceSessions.h"
#include "PlayfabInputValidationTest.generated.h"

UENUM()
enum class EValidationType : uint8
{
	Login,
	NewAccount,
	Federation,
	FederationLogin
};

USTRUCT()
struct FValidationStep
{
	GENERATED_BODY()

	FString ExpectedError;
	FString TestDescription;
	TFunction<void()> TestAction;

	FValidationStep() {}
	FValidationStep(const FString& InExpectedError, const FString& InTestDescription, TFunction<void()> InTestAction)
		: ExpectedError(InExpectedError)
		, TestDescription(InTestDescription)
		, TestAction(InTestAction)
	{}
};

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

	void RunValidationTests(EValidationType ValidationType);
	bool IsTestComplete() const { return bTestComplete; }
	bool GetTestPassed() const { return bTestPassed; }
	bool AreAllTestsComplete() const { return bAllTestsComplete; }
	FString GetLastError() const { return LastError; }
	void SetValidationType(EValidationType Type) { CurrentValidationType = Type; }
	EValidationType GetValidationType() const { return CurrentValidationType; }

	class FAutomationTestBase* ParentTest;

private:
	void InitializeAuthenticator();
	void SetupValidationSteps();
	TArray<FValidationStep> GetValidationStepsForType(EValidationType Type);

	bool bTestComplete = false;
	bool bTestPassed = false;
	bool bAllTestsComplete = false;
	FString LastError;
	USequenceSessions* Sessions;
	EValidationType CurrentValidationType;
	int32 CurrentStepIndex = -1;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForValidationTestComplete, UPlayFabInputValidationTestHelper*, TestHelper);
