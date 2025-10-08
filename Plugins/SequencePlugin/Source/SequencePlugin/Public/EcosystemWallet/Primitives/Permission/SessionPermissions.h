#pragma once

#include "CoreMinimal.h"
#include "Permission.h"
#include "Util/ByteArrayUtils.h"
#include "SessionPermissions.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FSessionPermissions
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FString SessionAddress;
	
	UPROPERTY()
	FString ChainId;

	UPROPERTY()
	FString ValueLimit;

	UPROPERTY()
	FString Deadline;

	UPROPERTY()
	TArray<FPermission> Permissions;

	static FSessionPermissions Decode(const TArray<uint8>& Data)
	{
		if (Data.Num() < 85)
		{
			UE_LOG(LogTemp, Error, TEXT("Data too short"));
			return FSessionPermissions();
		}

		// --- Signer (Address) ---
		TArray<uint8> SignerBytes;
		SignerBytes.Append(&Data[0], 20);
		FString Signer = FByteArrayUtils::BytesToHexString(SignerBytes);

		// --- ChainId (BigInt as FString) ---
		TArray<uint8> ChainIdBytes;
		ChainIdBytes.Append(&Data[20], 32);
		FString ChainId = FByteArrayUtils::BytesToBigIntHexString(SignerBytes);

		// --- ValueLimit (BigInt as FString) ---
		TArray<uint8> ValueLimitBytes;
		ValueLimitBytes.Append(&Data[52], 32);
		FString ValueLimit = FByteArrayUtils::BytesToBigIntHexString(SignerBytes);

		// --- Deadline (BigInt as FString) ---
		TArray<uint8> DeadlineBytes;
		DeadlineBytes.Append(&Data[84], 8);
		FString Deadline = FByteArrayUtils::BytesToBigIntHexString(SignerBytes);

		// --- Permissions ---
		if (Data.Num() <= 92)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid data: missing permissions length byte"));
			return FSessionPermissions();
		}

		uint8 PermissionsLength = Data[92];
		int32 Pointer = 93;

		TArray<FPermission> Permissions;
		Permissions.Reserve(PermissionsLength);

		for (int32 i = 0; i < PermissionsLength; i++)
		{
			FPermission Permission;
			int32 Consumed = 0;

			if (!FPermission::Decode(Data, Pointer, Permission, Consumed))
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to decode permission at index %d"), i);
				break;
			}

			Permissions.Add(Permission);
			Pointer += Consumed;
		}

		FSessionPermissions Result;
		Result.SessionAddress = Signer;
		Result.ChainId = ChainId;
		Result.ValueLimit = ValueLimit;
		Result.Deadline = Deadline;
		Result.Permissions = Permissions;
		return Result;
	}
};
