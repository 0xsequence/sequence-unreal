#pragma once

#include "CoreMinimal.h"
#include "EcosystemWallet/Signers/WalletSessions.h"
#include "Encryptors/GenericNativeEncryptor.h"
#include "UObject/Object.h"
#include "SessionStorage.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API USessionStorage : public UObject
{
	GENERATED_BODY()

public:
	USessionStorage();
	
	bool GetStoredSessions(FWalletSessions* Sessions);
	FWalletSessions GetStoredSessions();
	void StoreSessions(const FWalletSessions& WalletSessions);
	void AddSession(const FSessionCredentials& WalletSessions);
	void ClearSessions();

private:
	UPROPERTY()
	UGenericNativeEncryptor* Encryptor = nullptr;

	const FString SaveSlot = "SequenceCredentials";
	const uint32 UserIndex = 0;
};
