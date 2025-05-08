#pragma once
#include "CoreMinimal.h"
#include "Containers/Union.h"
#include "UObject/NoExportTypes.h"
#include "Util/Structs/BE_Structs.h"
#include "GenericPlatform/GenericPlatform.h"

using ByteLength = uint32;
using Hash = uint8*;
using TransactionUnion = TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction, FDelayedTransaction>;