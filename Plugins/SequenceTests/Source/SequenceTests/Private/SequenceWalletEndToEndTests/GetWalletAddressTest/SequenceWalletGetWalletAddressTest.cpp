// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Sequence/SequenceWallet.h"
#include "Engine/World.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FGetWalletAddressTest, "SequencePlugin.EndToEnd.SequenceWalletTests.GetWalletAddressTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)


void FGetWalletAddressTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Get Wallet Address Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FGetWalletAddressTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing Get Wallet Address"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {      
        if (TOptional<USequenceWallet*> OptionalSequenceWallet = USequenceWallet::Get(); OptionalSequenceWallet.IsSet() && OptionalSequenceWallet.GetValue())
        {
            const USequenceWallet * SequenceWallet = OptionalSequenceWallet.GetValue();
            const FString WalletAddress = SequenceWallet->GetWalletAddress();
            if (WalletAddress.Len() > 0)
            {
                AddInfo(FString::Printf(TEXT("Got Walletaddress: %s\nTest Complete"), *WalletAddress));
            }
            else
            {
                AddError(FString::Printf(TEXT("Failed to get walletaddress")));
            }          
        }
        else
        {
            AddError(FString::Printf(TEXT("Failed to get walletaddress")));
        }
        return true;
    }));
    return true;
}