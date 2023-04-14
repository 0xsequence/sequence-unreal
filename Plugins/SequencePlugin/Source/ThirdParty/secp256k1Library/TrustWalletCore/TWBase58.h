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

/// Base58 encode / decode functions
TW_EXPORT_STRUCT
struct TWBase58;

/// Encodes data as a Base58 string, including the checksum.
///
/// \param data The data to encode.
/// \return the encoded Base58 string with checksum.
TW_EXPORT_STATIC_METHOD
TWString *_Nonnull TWBase58Encode(TWData *_Nonnull data);

/// Encodes data as a Base58 string, not including the checksum.
///
/// \param data The data to encode.
/// \return then encoded Base58 string without checksum.
TW_EXPORT_STATIC_METHOD
TWString *_Nonnull TWBase58EncodeNoCheck(TWData *_Nonnull data);

/// Decodes a Base58 string, checking the checksum. Returns null if the string is not a valid Base58 string.
///
/// \param string The Base58 string to decode.
/// \return the decoded data, empty if the string is not a valid Base58 string with checksum.
TW_EXPORT_STATIC_METHOD
TWData *_Nullable TWBase58Decode(TWString *_Nonnull string);

/// Decodes a Base58 string, w/o checking the checksum. Returns null if the string is not a valid Base58 string.
///
/// \param string The Base58 string to decode.
/// \return the decoded data, empty if the string is not a valid Base58 string without checksum.
TW_EXPORT_STATIC_METHOD
TWData *_Nullable TWBase58DecodeNoCheck(TWString *_Nonnull string);

TW_EXTERN_C_END
