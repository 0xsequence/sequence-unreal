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

	FWalletSessions GetStoredSessions();
	bool GetStoredSessions(TArray<FSessionCredentials>& Sessions);
	void StoreSessions(const TArray<FSessionCredentials>& Sessions);
	void AddSession(const FSessionCredentials& Credentials);
	void ClearSessions();

private:
	UPROPERTY()
	UGenericNativeEncryptor* Encryptor = nullptr;

	const FString SaveSlot = "SequenceCredentials";
	const uint32 UserIndex = 0;
};
