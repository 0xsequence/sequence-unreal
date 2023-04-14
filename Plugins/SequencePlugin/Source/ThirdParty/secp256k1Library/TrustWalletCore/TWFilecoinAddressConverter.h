// Copyright © 2017-2023 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "TWBase.h"
#include "TWData.h"
#include "TWString.h"

TW_EXTERN_C_BEGIN

/// Filecoin-Ethereum address converter.
TW_EXPORT_STRUCT
struct TWFilecoinAddressConverter;

/// Converts a Filecoin address to Ethereum.
///
/// \param filecoinAddress: a Filecoin address.
/// \returns the Ethereum address. On invalid input empty string is returned. Returned object needs to be deleted after use.
TW_EXPORT_STATIC_METHOD
TWString* _Nonnull TWFilecoinAddressConverterConvertToEthereum(TWString* _Nonnull filecoinAddress);

/// Converts an Ethereum address to Filecoin.
///
/// \param ethAddress: an Ethereum address.
/// \returns the Filecoin address. On invalid input empty string is returned. Returned object needs to be deleted after use.
TW_EXPORT_STATIC_METHOD
TWString* _Nonnull TWFilecoinAddressConverterConvertFromEthereum(TWString* _Nonnull ethAddress);

TW_EXTERN_C_END
