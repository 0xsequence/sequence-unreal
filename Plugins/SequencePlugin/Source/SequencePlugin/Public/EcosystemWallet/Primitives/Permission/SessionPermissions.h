#pragma once

#include "CoreMinimal.h"
#include "Permission.h"
#include "EthAbi/EthAbiBridge.h"
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

	TArray<uint8> Encode()
	{
		TArray<uint8> EncodedAddress = FByteArrayUtils::PadLeft(FByteArrayUtils::HexStringToBytes(SessionAddress), 20);
		TArray<uint8> EncodedChainId = FByteArrayUtils::PadLeft(FEthAbiBridge::EncodeBigInteger(FEthAbiBridge::HexToBigIntString(ChainId)), 32);
		TArray<uint8> EncodedValueLimit = FByteArrayUtils::PadLeft(FEthAbiBridge::EncodeBigInteger(FEthAbiBridge::HexToBigIntString(ValueLimit)), 32);
		TArray<uint8> EncodedDeadline = FByteArrayUtils::PadLeft(FEthAbiBridge::EncodeBigInteger(FEthAbiBridge::HexToBigIntString(Deadline)), 8);
		TArray<uint8> EncodedPermissionsLen = TArray<uint8> { static_cast<uint8>(Permissions.Num()) };

		TArray<TArray<uint8>> EncodedPermissionsParts;
		for (FPermission Permission : Permissions)
		{
			EncodedPermissionsParts.Add(Permission.Encode());
		}

		TArray<uint8> EncodedPermissions = FByteArrayUtils::ConcatBytes(EncodedPermissionsParts);
		
		return FByteArrayUtils::ConcatBytes({EncodedAddress, EncodedChainId, EncodedValueLimit, EncodedDeadline, EncodedPermissionsLen, EncodedPermissions});
	}

	static FSessionPermissions Decode(const TArray<uint8>& Data)
	{
		if (Data.Num() < 85)
		{
			UE_LOG(LogTemp, Error, TEXT("Data too short"));
			return FSessionPermissions();
		}

		TArray<uint8> SignerBytes;
		SignerBytes.Append(&Data[0], 20);
		FString Signer = FByteArrayUtils::BytesToHexString(SignerBytes);

		TArray<uint8> ChainIdBytes;
		ChainIdBytes.Append(&Data[20], 32);
		FString ChainId = FByteArrayUtils::BytesToBigIntHexString(ChainIdBytes);

		TArray<uint8> ValueLimitBytes;
		ValueLimitBytes.Append(&Data[52], 32);
		FString ValueLimit = FByteArrayUtils::BytesToBigIntHexString(ValueLimitBytes);

		TArray<uint8> DeadlineBytes;
		DeadlineBytes.Append(&Data[84], 8);
		FString Deadline = FByteArrayUtils::BytesToBigIntHexString(DeadlineBytes);

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
