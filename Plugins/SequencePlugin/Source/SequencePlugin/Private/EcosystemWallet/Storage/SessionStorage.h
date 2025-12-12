#pragma once

#include "CoreMinimal.h"
#include "EcosystemWallet/Signers/SessionSigner.h"
#include "EcosystemWallet/Signers/WalletSessions.h"
#include "Encryptors/GenericNativeEncryptor.h"
#include "UObject/Object.h"

class SEQUENCEPLUGIN_API FSessionStorage
{
public:
	static TArray<FSessionSigner> GetStoredSigners();
	static FWalletSessions GetStoredSessions();
	static bool GetStoredSessions(TArray<FSessionCredentials>& Sessions);
	static void StoreSessions(const TArray<FSessionCredentials>& Sessions);
	static void AddSession(const FSessionCredentials& Credentials);
	static void ClearSessions();

private:
	inline static UGenericNativeEncryptor* GetEncryptor();

	static inline const FString SaveSlot = "SequenceCredentials";
	static inline const uint32 UserIndex = 0;
};
