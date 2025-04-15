#pragma once

UENUM(Blueprintable)
enum class ECheckoutOptions : uint8
{
	CryptoPurchase			UMETA(DisplayName = "CryptoPurchase"),
	SwapAndPay				UMETA(DisplayName = "SwapAndPay"),
	SardineCheckout			UMETA(DisplayName = "SardineCheckout"),
	TransferFundsViaQR		UMETA(DisplayName = "TransferFundsViaQR"),
	TransakOnramp			UMETA(DisplayName = "TransakOnramp")
};