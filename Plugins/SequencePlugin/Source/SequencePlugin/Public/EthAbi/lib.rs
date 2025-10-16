use secp256k1::{
    ecdsa::RecoverableSignature,
    ecdsa::RecoveryId,
    Message, Secp256k1, SecretKey,
    PublicKey
};
use tiny_keccak::{Hasher, Keccak};
use ethabi::ethereum_types::H160;
use ethers::types::transaction::eip712::{Eip712, TypedData};
use ethers::utils::keccak256;
use ethabi::{encode, decode as abi_decode, Address, Function, Token, Param, ParamType, StateMutability};
use serde_json::{json, Map, Value};
use serde::Deserialize;
use hex::FromHex;
use std::ffi::{CStr, CString};
use hex::decode;
use std::collections::HashMap;
use num_bigint::BigUint;
use num_traits::Num;
use std::os::raw::{c_char, c_uchar, c_int};
use std::ptr;
use std::slice;

#[derive(Deserialize)]
struct AbiValue {
    #[serde(rename = "type")]
    kind: String,
    value: String,
}

/// Converts a BigInteger (as a decimal or hex string) into big-endian bytes.
/// Interprets the input as an integer value, not a UTF-8 sequence of digits.
/// Returns 0 on success, -1 on invalid input, -2 if too large.
#[no_mangle]
pub extern "C" fn bigint_to_bytes2(
    value_ptr: *const u8,
    value_len: usize,
    size: usize,
    out_bytes: *mut u8,
    out_len: *mut usize,
) -> i32 {
    let value_str = match std::str::from_utf8(unsafe { slice::from_raw_parts(value_ptr, value_len) }) {
        Ok(v) => v.trim(),
        Err(_) => return -1,
    };

    // Detect base automatically
    let value = if let Some(stripped) = value_str.strip_prefix("0x") {
        BigUint::from_str_radix(stripped, 16)
    } else {
        BigUint::from_str_radix(value_str, 10)
    };

    let value = match value {
        Ok(v) => v,
        Err(_) => return -1,
    };

    let mut raw = value.to_bytes_be();

    // Apply fixed size
    if size > 0 {
        if raw.len() > size {
            return -2;
        }
        let mut padded = vec![0u8; size - raw.len()];
        padded.extend_from_slice(&raw);
        raw = padded;
    }

    unsafe {
        ptr::copy_nonoverlapping(raw.as_ptr(), out_bytes, raw.len());
        *out_len = raw.len();
    }

    0
}

#[no_mangle]
pub extern "C" fn recover_eth_pub_and_address(
    sig_ptr: *const u8,
    sig_len: usize,
    hash_ptr: *const u8,
    hash_len: usize,
    out_pubkey: *mut u8,   // expects 64 bytes (X || Y)
    out_address: *mut u8,  // expects 20 bytes
) -> i32 {
    // Safety wrappers for incoming pointers
    let sig = unsafe { slice::from_raw_parts(sig_ptr, sig_len) };
    let hash = unsafe { slice::from_raw_parts(hash_ptr, hash_len) };

    if sig.len() != 65 || hash.len() != 32 {
        return -1;
    }

    let secp = Secp256k1::new();

    // Split signature into r,s,v
    let recovery_id = match sig[64] {
        27 | 28 => RecoveryId::from_i32((sig[64] - 27) as i32).unwrap(),
        0 | 1 => RecoveryId::from_i32(sig[64] as i32).unwrap(),
        _ => return -1,
    };

    let rec_sig = match RecoverableSignature::from_compact(&sig[0..64], recovery_id) {
        Ok(s) => s,
        Err(_) => return -1,
    };

    let msg = match Message::from_slice(hash) {
        Ok(m) => m,
        Err(_) => return -1,
    };

    let pubkey = match secp.recover_ecdsa(&msg, &rec_sig) {
        Ok(p) => p,
        Err(_) => return -1,
    };

    // Serialize pubkey uncompressed (0x04 + 64 bytes)
    let pub_uncompressed = pubkey.serialize_uncompressed();

    // Compute Ethereum address
    let mut keccak = Keccak::v256();
    let mut hash32 = [0u8; 32];
    keccak.update(&pub_uncompressed[1..]); // skip 0x04
    keccak.finalize(&mut hash32);

    let address = &hash32[12..]; // last 20 bytes

    unsafe {
        ptr::copy_nonoverlapping(pub_uncompressed[1..].as_ptr(), out_pubkey, 64); // X,Y
        ptr::copy_nonoverlapping(address.as_ptr(), out_address, 20);
    }

    0 // success
}

#[no_mangle]
pub extern "C" fn encode_two_addresses_ethabi(
    signer_ptr: *const u8, // 20 bytes
    value_ptr: *const u8,  // 20 bytes
    out_ptr: *mut u8,      // output buffer (at least 64 bytes)
) -> usize {
    unsafe {
        let signer = H160::from_slice(std::slice::from_raw_parts(signer_ptr, 20));
        let value  = H160::from_slice(std::slice::from_raw_parts(value_ptr, 20));

        let encoded = encode(&[
            Token::Address(signer),
            Token::Address(value),
        ]);

        std::ptr::copy_nonoverlapping(encoded.as_ptr(), out_ptr, encoded.len());
        encoded.len()
    }
}

#[no_mangle]
pub extern "C" fn encode_and_hash_typed_data(
    domain_json: *const c_char,
    out_hash: *mut c_uchar, // pointer to 32-byte buffer allocated by caller
) {
    // Convert C string to Rust string
    let domain_json = unsafe {
        assert!(!domain_json.is_null());
        CStr::from_ptr(domain_json)
            .to_str()
            .expect("Invalid UTF-8 in domain_json")
    };

    // Parse EIP-712 TypedData from JSON
    let typed_data: TypedData = serde_json::from_str(domain_json)
        .expect("Invalid EIP-712 JSON");

    // encode_eip712() already returns a [u8; 32] struct hash
    let hash_struct = typed_data.encode_eip712().expect("EIP712 encoding failed");

    // Copy the 32-byte result into Unreal’s output buffer
    unsafe {
        std::ptr::copy_nonoverlapping(hash_struct.as_ptr(), out_hash, 32);
    }
}

#[no_mangle]
pub extern "C" fn sign_recoverable(
    hash_ptr: *const c_uchar,
    hash_len: usize,           // must be 32
    priv_ptr: *const c_uchar,  // 32
    priv_len: usize,
) -> *mut c_char {
    // Safety: treat pointers as slices
    let hash = unsafe { std::slice::from_raw_parts(hash_ptr, hash_len) };
    let sk_bytes = unsafe { std::slice::from_raw_parts(priv_ptr, priv_len) };

    if hash.len() != 32 || sk_bytes.len() != 32 {
        return CString::new("").unwrap().into_raw();
    }

    let secp = Secp256k1::new();

    let msg = match Message::from_digest_slice(hash) {
        Ok(m) => m,
        Err(_) => return CString::new("").unwrap().into_raw(),
    };

    let sk = match SecretKey::from_slice(sk_bytes) {
        Ok(k) => k,
        Err(_) => return CString::new("").unwrap().into_raw(),
    };

    // Sign (recoverable)
    let sig: RecoverableSignature = secp.sign_ecdsa_recoverable(&msg, &sk);

    // Serialize compact (64 bytes) + recid
    let (rec_id, sig64) = sig.serialize_compact();
    let v: u8 = 27 + rec_id.to_i32() as u8;

    // r || s || v (65 bytes) as 0x hex
    let mut out = Vec::with_capacity(65);
    out.extend_from_slice(&sig64[..]);
    out.push(v);

    let hex_str = format!("0x{}", hex::encode(out));
    CString::new(hex_str).unwrap().into_raw()
}

#[no_mangle]
pub extern "C" fn encode_function_call(signature: *const c_char, args_json: *const c_char) -> *mut c_char {
    unsafe {
        if signature.is_null() || args_json.is_null() {
            return empty_cstring();
        }
    }

    // Step 1: Read C strings
    let signature = unsafe {
        CStr::from_ptr(signature).to_str().unwrap_or("")
    };
    let args_json = unsafe {
        CStr::from_ptr(args_json).to_str().unwrap_or("")
    };

    // Step 2: Parse function signature
    let function = match parse_signature(signature) {
        Ok(f) => f,
        Err(_) => return empty_cstring(),
    };

    // Step 3: Parse JSON arguments
    let json_args: Vec<Value> = match serde_json::from_str(args_json) {
        Ok(v) => v,
        Err(_) => return empty_cstring(),
    };

    // Step 4: Convert to ABI tokens
    let tokens: Vec<Token> = match json_args.iter().zip(function.inputs.iter())
        .map(|(value, param)| parse_token(value, &param.kind))
        .collect::<Result<_, _>>() {
        Ok(t) => t,
        Err(_) => return empty_cstring(),
    };

    // Step 5: Encode ABI
    let encoded = match function.encode_input(&tokens) {
        Ok(bytes) => bytes,
        Err(_) => return empty_cstring(),
    };

    let hex_string = format!("0x{}", hex::encode(encoded));
    CString::new(hex_string).unwrap().into_raw()
}

#[no_mangle]
pub extern "C" fn decode_function_result(abi_json: *const c_char, result_hex: *const c_char) -> *mut c_char {
    // Convert C strings to Rust strings
    let abi_json = unsafe {
        if abi_json.is_null() { return empty_cstring(); }
        CStr::from_ptr(abi_json).to_str().unwrap_or("")
    };
    let result_hex = unsafe {
        if result_hex.is_null() { return empty_cstring(); }
        CStr::from_ptr(result_hex).to_str().unwrap_or("")
    };

    // Decode and handle errors
    let function: Function = match serde_json::from_str(abi_json) {
        Ok(f) => f,
        Err(_) => return empty_cstring(),
    };

    let result_bytes = match decode(result_hex.strip_prefix("0x").unwrap_or(result_hex)) {
        Ok(b) => b,
        Err(_) => return empty_cstring(),
    };

    let output_tokens = match function.decode_output(&result_bytes) {
        Ok(t) => t,
        Err(_) => return empty_cstring(),
    };

    let json_obj = tokens_to_array_json(&output_tokens, &function.outputs);
    let json_string = match serde_json::to_string(&json_obj) {
        Ok(s) => s,
        Err(_) => return empty_cstring(),
    };

    CString::new(json_string).unwrap().into_raw()
}

#[no_mangle]
pub extern "C" fn encode_bigint_to_bytes(
    num_str: *const c_char,
    out_len: *mut c_int,
) -> *mut c_uchar {
if num_str.is_null() || out_len.is_null() {
        return ptr::null_mut();
    }

    // SAFETY: We checked that num_str is non-null.
    let c_str = unsafe { CStr::from_ptr(num_str) };

    let num_str = match c_str.to_str() {
        Ok(s) => s,
        Err(_) => return ptr::null_mut(),
    };

    let bigint = match BigUint::from_str_radix(num_str, 10) {
        Ok(n) => n,
        Err(_) => return ptr::null_mut(),
    };

    let bytes = bigint.to_bytes_be();
    let len = bytes.len() as c_int;

    // SAFETY: We checked that out_len is non-null.
    unsafe {
        *out_len = len;
    }

    // SAFETY: Allocating a buffer with libc::malloc for C compatibility.
    let buf = unsafe { libc::malloc(len as usize) as *mut c_uchar };
    if buf.is_null() {
        return ptr::null_mut();
    }

    // SAFETY: buf is valid, bytes is valid, len is correct.
    unsafe {
        std::ptr::copy_nonoverlapping(bytes.as_ptr(), buf, len as usize);
    }

    buf
}

#[no_mangle]
pub extern "C" fn hex_to_bigint_decimal(hex_cstr: *const c_char) -> *mut c_char {
    // Safety: only unsafe part is reading the C string.
    // The function itself has no unsafe logic beyond that.
    if hex_cstr.is_null() {
        return std::ptr::null_mut();
    }

    let hex_str = unsafe {
        match CStr::from_ptr(hex_cstr).to_str() {
            Ok(s) => s,
            Err(_) => return std::ptr::null_mut(),
        }
    };

    // Strip optional 0x prefix
    let cleaned = hex_str.trim_start_matches("0x").trim_start_matches("0X");

    // Parse hex → BigUint
    let value = match BigUint::from_str_radix(cleaned, 16) {
        Ok(v) => v,
        Err(_) => return std::ptr::null_mut(),
    };

    // Convert to decimal string
    let decimal_str = value.to_str_radix(10);

    // Convert Rust string → C string
    match CString::new(decimal_str) {
        Ok(c_string) => c_string.into_raw(),
        Err(_) => std::ptr::null_mut(),
    }
}

#[no_mangle]
pub extern "C" fn abi_encode_json(
    json_ptr: *const c_char,
    out_len: *mut usize,
) -> *mut c_uchar {
    // Convert C string from Unreal
    let json_str = unsafe {
        assert!(!json_ptr.is_null());
        CStr::from_ptr(json_ptr).to_str().unwrap()
    };

    // Parse JSON array of ABI values
    let values: Vec<AbiValue> = serde_json::from_str(json_str).unwrap();

    // Convert to ethabi Tokens
    let mut tokens = Vec::new();
    for v in values {
        match v.kind.as_str() {
            "address" => {
                let addr: Address = v.value.parse().unwrap();
                tokens.push(Token::Address(addr));
            }
            "bytes" => {
                let s = v.value.strip_prefix("0x").unwrap_or(&v.value);
                let bytes = hex::decode(s).unwrap();
                tokens.push(Token::Bytes(bytes));
            }
            "string" => {
                tokens.push(Token::String(v.value));
            }
            "uint256" => {
                use ethabi::ethereum_types::U256;
                let num = U256::from_dec_str(&v.value).unwrap();
                tokens.push(Token::Uint(num));
            }
            "bool" => {
                let b = match v.value.as_str() {
                    "true" | "1" => true,
                    "false" | "0" => false,
                    _ => panic!("Invalid bool value"),
                };
                tokens.push(Token::Bool(b));
            }
            other => panic!("Unsupported type: {}", other),
        }
    }

    // Encode via ethabi (standard ABI encoding)
    let encoded = encode(&tokens);
    let len = encoded.len();
    unsafe { *out_len = len };

    // Return heap-allocated buffer to Unreal
    let buf = encoded.into_boxed_slice();
    Box::into_raw(buf) as *mut c_uchar
}

#[no_mangle]
pub extern "C" fn abi_decode_types_json(
    raw_ptr: *const c_uchar,
    raw_len: usize,
    types_json_ptr: *const c_char,
) -> *mut c_char {
    // Safety: convert pointers to Rust types
    let raw = unsafe { slice::from_raw_parts(raw_ptr, raw_len) };
    let types_json = unsafe {
        assert!(!types_json_ptr.is_null());
        CStr::from_ptr(types_json_ptr).to_str().unwrap()
    };

    // Parse ["address","bytes","bytes"]
    let type_list: Vec<String> = serde_json::from_str(types_json).unwrap();

    // Convert to ParamType
    let param_types: Vec<ParamType> = type_list
        .iter()
        .map(|t| match t.as_str() {
            "address" => ParamType::Address,
            "bytes" => ParamType::Bytes,
            "string" => ParamType::String,
            "uint256" => ParamType::Uint(256),
            "bool" => ParamType::Bool,
            other => panic!("Unsupported type: {}", other),
        })
        .collect();

    // Decode ABI-encoded data
    let tokens = abi_decode(&param_types, raw).expect("Failed to decode data");

    // Convert each token → hex string
    let mut hex_values = Vec::new();
    for token in tokens {
        let hex_str = match token {
            Token::Address(addr) => format!("0x{:x}", addr),
            Token::Bytes(b) => format!("0x{}", hex::encode(b)),
            Token::Uint(u) => {
                // ABI uint256 → 32-byte big-endian
                let mut buf = [0u8; 32];
                u.to_big_endian(&mut buf);
                format!("0x{}", hex::encode(buf))
            }
            Token::Bool(b) => {
                let v = if b { "01" } else { "00" };
                format!("0x{}", v)
            }
            Token::String(s) => {
                format!("0x{}", hex::encode(s.as_bytes()))
            }
            _ => "0x".to_string(),
        };
        hex_values.push(hex_str);
    }

    // Return JSON array of hex strings
    let json_out = serde_json::to_string(&hex_values).unwrap();
    let cstring = CString::new(json_out).unwrap();
    cstring.into_raw()
}

#[no_mangle]
pub extern "C" fn free_encoded_bytes(ptr: *mut c_uchar) {
    if ptr.is_null() {
        return;
    }

    unsafe {
        libc::free(ptr as *mut libc::c_void);
    }
}

#[no_mangle]
pub extern "C" fn free_encoded_bytes_raw(ptr: *mut c_uchar, len: usize) {
    if !ptr.is_null() {
        unsafe {
            let _ = Vec::from_raw_parts(ptr, len, len);
        }
    }
}

#[no_mangle]
pub extern "C" fn free_string(s: *mut c_char) {
    if !s.is_null() {
        unsafe {
            drop(CString::from_raw(s));
        }
    }
}

fn empty_cstring() -> *mut c_char {
    CString::new("").unwrap().into_raw()
}

fn tokens_to_array_json(tokens: &[Token], params: &[Param]) -> Value {
    let values: Vec<Value> = tokens
        .iter()
        .zip(params.iter())
        .map(|(t, ty)| token_to_json(t, &ty.kind))
        .collect();
        
    Value::Array(values)
}

fn parse_token(value: &serde_json::Value, kind: &ethabi::ParamType) -> Result<ethabi::Token, ()> {
    use ethabi::{ethereum_types::*, ParamType, Token};
    use serde_json::Value;

    match (kind, value) {
        // Tuples
        (ParamType::Tuple(types), Value::Array(vals)) => {
            if vals.len() != types.len() {
                return Err(());
            }
            let tokens = vals.iter()
                .zip(types.iter())
                .map(|(v, t)| parse_token(v, t))
                .collect::<Result<Vec<_>, _>>()?;
            Ok(Token::Tuple(tokens))
        }

        // Arrays
        (ParamType::Array(inner), Value::Array(vals)) => {
            let tokens = vals.iter().map(|v| parse_token(v, inner)).collect::<Result<Vec<_>, _>>()?;
            Ok(Token::Array(tokens))
        }
        (ParamType::FixedArray(inner, len), Value::Array(vals)) => {
            if vals.len() != *len {
                return Err(());
            }
            let tokens = vals.iter().map(|v| parse_token(v, inner)).collect::<Result<Vec<_>, _>>()?;
            Ok(Token::FixedArray(tokens))
        }

        // Uint
        (ParamType::Uint(_), Value::Number(n)) => Ok(Token::Uint(U256::from(n.as_u64().ok_or(())?))),
        (ParamType::Uint(_), Value::String(s)) => {
            let s = s.trim_start_matches("0x");
            U256::from_dec_str(s)
                .or_else(|_| U256::from_str_radix(s, 16))
                .map(Token::Uint)
                .map_err(|_| ())
        }

        // Int
        (ParamType::Int(_), Value::Number(n)) => {
            let v = n.as_i64().ok_or(())?;
            let u = if v < 0 {
                U256::MAX - U256::from((-v) as u64) + 1
            } else {
                U256::from(v as u64)
            };
            Ok(Token::Int(u))
        }
        (ParamType::Int(_), Value::String(s)) => {
            let s = s.trim();
            let neg = s.starts_with('-');
            let u = U256::from_dec_str(s.trim_start_matches('-').trim_start_matches("0x"))
                .or_else(|_| U256::from_str_radix(s.trim_start_matches('-').trim_start_matches("0x"), 16))
                .map_err(|_| ())?;
            let val = if neg { U256::MAX - u + 1 } else { u };
            Ok(Token::Int(val))
        }

        // Bool
        (ParamType::Bool, Value::Bool(b)) => Ok(Token::Bool(*b)),
        (ParamType::Bool, Value::String(s)) => {
            let s = s.trim().to_ascii_lowercase();
            match s.as_str() {
                "true" | "1" => Ok(Token::Bool(true)),
                "false" | "0" => Ok(Token::Bool(false)),
                _ => Err(()),
            }
        }

        // Address
        (ParamType::Address, Value::String(s)) => {
            let s = s.trim_start_matches("0x");
            let bytes = hex::decode(s).map_err(|_| ())?;
            if bytes.len() != 20 {
                return Err(());
            }
            Ok(Token::Address(Address::from_slice(&bytes)))
        }

        // Strings
        (ParamType::String, Value::String(s)) => Ok(Token::String(s.clone())),

        // Bytes and fixed bytes
        (ParamType::Bytes, Value::String(s)) => {
            let s = s.trim();
            let hex_str = s.strip_prefix("0x").or(s.strip_prefix("0X")).unwrap_or(s);
            if hex_str.is_empty() {
                return Ok(Token::Bytes(vec![]));
            }
            if hex_str.len() % 2 == 1 {
                let mut owned = String::with_capacity(hex_str.len() + 1);
                owned.push('0');
                owned.push_str(hex_str);
                let bytes = hex::decode(&owned).map_err(|_| ())?;
                return Ok(Token::Bytes(bytes));
            }
            let bytes = hex::decode(hex_str).map_err(|_| ())?;
            Ok(Token::Bytes(bytes))
        }
        (ParamType::FixedBytes(len), Value::String(s)) => {
            let bytes = hex::decode(s.trim_start_matches("0x")).map_err(|_| ())?;
            if bytes.len() != *len {
                return Err(());
            }
            Ok(Token::FixedBytes(bytes))
        }

        _ => Err(()),
    }
}


// Split arguments by commas at top level (handles tuples like (a,b),(c,d))
fn split_top_level(s: &str, delim: char) -> Result<Vec<String>, ()> {
    let mut out = Vec::new();
    let mut depth = 0usize;
    let mut cur = String::new();
    for ch in s.chars() {
        match ch {
            '(' | '[' => { depth += 1; cur.push(ch); }
            ')' | ']' => { depth = depth.checked_sub(1).ok_or(())?; cur.push(ch); }
            _ if ch == delim && depth == 0 => {
                if !cur.trim().is_empty() { out.push(cur.trim().to_string()); }
                cur.clear();
            }
            _ => cur.push(ch),
        }
    }
    if !cur.trim().is_empty() { out.push(cur.trim().to_string()); }
    Ok(out)
}

fn token_to_json(token: &Token, param_type: &ParamType) -> Value {
    match (token, param_type) {
        (Token::Uint(n), ParamType::Uint(_)) |
        (Token::Int(n), ParamType::Int(_)) => {
            // Always return as string to preserve full 256-bit precision
            Value::String(n.to_string())
        }
        (Token::Address(addr), ParamType::Address) => {
            Value::String(format!("0x{}", hex::encode(addr)))
        }
        (Token::Bool(b), ParamType::Bool) => Value::Bool(*b),
        (Token::String(s), ParamType::String) => Value::String(s.clone()),
        (Token::Bytes(b), ParamType::Bytes) |
        (Token::FixedBytes(b), ParamType::FixedBytes(_)) => {
            Value::String(format!("0x{}", hex::encode(b)))
        }
        (Token::Array(arr), ParamType::Array(inner)) => {
            Value::Array(arr.iter().map(|t| token_to_json(t, inner)).collect())
        }
        (Token::FixedArray(arr), ParamType::FixedArray(inner, _)) => {
            Value::Array(arr.iter().map(|t| token_to_json(t, inner)).collect())
        }
        (Token::Tuple(tokens), ParamType::Tuple(types)) => {
            Value::Array(tokens.iter()
                .zip(types.iter())
                .map(|(t, ty)| token_to_json(t, ty))
                .collect())
        }
        _ => Value::Null,
    }
}

fn parse_signature(signature: &str) -> Result<Function, ()> {
    let open = signature.find('(').ok_or(())?;
    // find the matching ')' for the first '('
    let mut depth = 0usize;
    let mut close = None;
    for (i, ch) in signature.char_indices().skip(open) {
        match ch {
            '(' => depth += 1,
            ')' => {
                depth -= 1;
                if depth == 0 {
                    close = Some(i);
                    break;
                }
            }
            _ => {}
        }
    }
    let close = close.ok_or(())?;

    let name = &signature[..open];
    let args_str = &signature[open + 1..close];
    let arg_types: Vec<ParamType> = if args_str.trim().is_empty() {
        vec![]
    } else {
        split_top_level(args_str, ',')?
            .iter()
            .map(|s| parse_param_type(s.trim()))
            .collect::<Result<_, _>>()?
    };

    let inputs = arg_types.into_iter().enumerate().map(|(i, kind)| Param {
        name: format!("arg{}", i),
        kind,
        internal_type: None,
    }).collect();

    Ok(Function {
        name: name.to_string(),
        inputs,
        outputs: vec![],
        constant: true,
        state_mutability: StateMutability::View,
    })
}

fn parse_param_type(s: &str) -> Result<ParamType, ()> {
    use ethabi::ParamType;
    let s = s.trim();

    // tuple: ( ... )
    if s.starts_with('(') && s.ends_with(')') {
        let inner = &s[1..s.len() - 1];
        let inner_types = split_top_level(inner, ',')?
            .into_iter()
            .map(|t| parse_param_type(&t))
            .collect::<Result<Vec<_>, _>>()?;
        return Ok(ParamType::Tuple(inner_types));
    }

    // arrays: type[], type[3], possibly nested (e.g. (uint,bool)[])
    if let Some(idx) = s.find('[') {
        if !s.ends_with(']') { return Err(()); }
        let inner_ty = parse_param_type(&s[..idx])?;
        let len_str = &s[idx+1..s.len()-1];
        return if len_str.is_empty() {
            Ok(ParamType::Array(Box::new(inner_ty)))
        } else if let Ok(n) = len_str.parse::<usize>() {
            Ok(ParamType::FixedArray(Box::new(inner_ty), n))
        } else {
            Err(())
        };
    }

    // primitives
    match s {
        "address" => Ok(ParamType::Address),
        "bool"    => Ok(ParamType::Bool),
        "string"  => Ok(ParamType::String),
        "bytes"   => Ok(ParamType::Bytes),

        _ if s.starts_with("uint") => {
            let size = s[4..].parse::<usize>().unwrap_or(256);
            Ok(ParamType::Uint(size))
        }
        _ if s.starts_with("int") => {
            let size = s[3..].parse::<usize>().unwrap_or(256);
            Ok(ParamType::Int(size))
        }
        _ if s.starts_with("bytes") => {
            // bytes1..bytes32
            let n = s[5..].parse::<usize>().map_err(|_| ())?;
            if (1..=32).contains(&n) { Ok(ParamType::FixedBytes(n)) } else { Err(()) }
        }

        _ => Err(()),
    }
}
