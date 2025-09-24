#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EcosystemWallet/Primitives/Calls/Calls.h"
#include "Transaction.generated.h"

struct FCall;

UINTERFACE()
class USeqTransactionBase : public UInterface
{
	GENERATED_BODY()
};

class SEQUENCEPLUGIN_API ISeqTransactionBase
{
	GENERATED_BODY()

public:
	virtual FCalls GetCalls();
};
