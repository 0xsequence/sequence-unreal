#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Sequence/SequenceSessions.h"
#include "SignInOutRepeatedlyTest.generated.h"

class FSignInOutRepeatedlyTest;

UCLASS()
class USignInOutRepeatedlyTestHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnAuthSuccess();

	UFUNCTION()
	void OnAuthFailure(const FString& Error);

	void RunTest();
	bool IsTestComplete() const { return bTestComplete; }

	FSignInOutRepeatedlyTest* ParentTest;

private:
	bool bTestComplete = false;
	USequenceSessions* Sessions = nullptr;
	int32 Repetitions = 0;
	static const int32 MaxRepetitions = 3;
	TArray<FString> SessionIds;

	void ConnectAsGuest();
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForSignInOutCompletion, USignInOutRepeatedlyTestHelper*, TestHelper);
