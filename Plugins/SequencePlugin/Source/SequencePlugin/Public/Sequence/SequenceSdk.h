#pragma once

#include "CoreMinimal.h"

/*
 * Central class for any SDK configuration information.
 */
class SEQUENCEPLUGIN_API SequenceSdk
{
public:
	/**
	 * Sets the chain ID for the current blockchain network context.
	 *
	 * @param NewChainId A 64-bit integer representing the new chain ID to be used (e.g., 1 for Ethereum, 137 for Polygon).
	 */
	static void SetChainId(int64 NewChainId);

	/**
	 * Sets the redirect URL to be used after completing an on-ramp transaction.
	 *
	 * @param NewRedirectUrl A string representing the new redirect URL used during Google- & Apple Sign-In.
	 */
	static void SetRedirectUrl(const FString& NewRedirectUrl);

	/**
	 * Retrieves the numeric chain ID of the currently configured blockchain network.
	 *
	 * @return A 64-bit integer representing the chain ID (e.g., 1 for Ethereum Mainnet, 137 for Polygon).
	 */
	static int64 GetChainId();

	/**
	 * Retrieves the chain ID of the currently configured blockchain network as a string.
	 *
	 * @return A string representing the chain ID (e.g., "1" for Ethereum Mainnet, "137" for Polygon).
	 */
	static FString GetChainIdString();

	/**
	 * Retrieves the name of the currently configured blockchain network.
	 *
	 * @return A string representing the name of the active blockchain (e.g., "Ethereum", "Immutable").
	 */
	static FString GetChainName();

	/**
	 * Retrieves the predefined redirect URL used after completing an on-ramp transaction.
	 *
	 * @return A string representing the redirect URL, used to return the user to the application after external interaction.
	 */
	static FString GetRedirectUrl();
private:
	inline static int64 ChainId = -1;
	inline static FString RedirectUrl = "";
};
