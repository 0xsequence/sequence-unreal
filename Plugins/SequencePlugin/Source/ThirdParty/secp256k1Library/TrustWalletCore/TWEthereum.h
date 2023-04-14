// Copyright © 2017-2023 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "TWBase.h"
#include "TWString.h"

TW_EXTERN_C_BEGIN

TW_EXPORT_STRUCT
struct TWEthereum;

/// Generate a layer 2 eip2645 derivation path from eth address, layer, application and given index.
///
/// \param wallet non-null TWHDWallet
/// \param ethAddress non-null Ethereum address
/// \param layer  non-null layer 2 name (E.G starkex)
/// \param application non-null layer 2 application (E.G immutablex)
/// \param index non-null layer 2 index (E.G 1)
/// \return a valid eip2645 layer 2 derivation path as a string
TW_EXPORT_STATIC_METHOD
TWString* _Nonnull TWEthereumEip2645GetPath(TWString* _Nonnull ethAddress, TWString* _Nonnull layer,  TWString* _Nonnull application,  TWString* _Nonnull index);

/// Generates a deployment address for a ERC-4337 compatible smart contract wallet
///
/// \param factoryAddress non-null address of the account factory
/// \param logicAddress non-null address of the wallet's logic smart contract
/// \param ownerAddress  non-null address of the signing key that controls the smart contract wallet
/// \return Ethereum resulting address
TW_EXPORT_STATIC_METHOD
TWString* _Nonnull TWEthereumEip4337GetDeploymentAddress(TWString* _Nonnull factoryAddress, TWString* _Nonnull logicAddress, TWString* _Nonnull ownerAddress);
TW_EXTERN_C_END
