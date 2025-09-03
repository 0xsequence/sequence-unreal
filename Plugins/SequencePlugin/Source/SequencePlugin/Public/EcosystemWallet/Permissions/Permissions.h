#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Permissions.generated.h"

struct FSessionPermissions;

UINTERFACE()
class UPermissions : public UInterface
{
	GENERATED_BODY()
};

class SEQUENCEPLUGIN_API IPermissions
{
	GENERATED_BODY()

public:
	virtual FSessionPermissions GetSessionPermissions();
};
