#pragma once
#include "EcosystemWallet/Primitives/Permission/Permission.h"
#include "JsonObjectConverter.h"
#include "SessionArgs.generated.h"

USTRUCT()
struct FSessionArgs
{
	GENERATED_BODY()

	FString SessionAddress;
	TOptional<FString> ChainId;
	TOptional<FString> ValueLimit;
	TOptional<FString> Deadline;
	TOptional<TArray<FPermission>> Permissions;

	FSessionArgs() {}
	explicit FSessionArgs(const FString& InSessionAddress)
		: SessionAddress(InSessionAddress)
	{}

	FSessionArgs(const FString& InSessionAddress, const FString& InChainId, const FString& InValueLimit, const FString& InDeadline, const TArray<FPermission>& InPermissions)
		: SessionAddress(InSessionAddress)
		, ChainId(InChainId)
		, ValueLimit(InValueLimit)
		, Deadline(InDeadline)
		, Permissions(InPermissions)
	{}

	TSharedPtr<FJsonObject> ToJson() const
	{
		TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();

		JsonObject->SetStringField(TEXT("sessionAddress"), SessionAddress);

		if (ChainId.IsSet())
		{
			JsonObject->SetStringField(TEXT("chainId"), ChainId.GetValue());
		}

		if (ValueLimit.IsSet())
		{
			JsonObject->SetStringField(TEXT("valueLimit"), ValueLimit.GetValue());
		}

		if (Deadline.IsSet())
		{
			JsonObject->SetStringField(TEXT("deadline"), Deadline.GetValue());
		}

		if (Permissions.IsSet())
		{
			TArray<TSharedPtr<FJsonValue>> PermissionArray;
			for (const FPermission& Permission : Permissions.GetValue())
			{
				TSharedPtr<FJsonObject> PermObj = FJsonObjectConverter::UStructToJsonObject(Permission);
				PermissionArray.Add(MakeShared<FJsonValueObject>(PermObj));
			}
			
			JsonObject->SetArrayField(TEXT("permissions"), PermissionArray);
		}

		return JsonObject;
	}
};
