use secp256k1::{
    ecdsa::RecoverableSignature,
    ecdsa::RecoveryId,
    Message, Secp256k1, SecretKey,
};
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
    // Step 1: Read C strings
    let signature = unsafe {
        if signature.is_null() {
            return empty_cstring();
        }
        CStr::from_ptr(signature).to_str().unwrap_or("")
    };

    let args_json = unsafe {
        if args_json.is_null() {
            return empty_cstring();
        }
        CStr::from_ptr(args_json).to_str().unwrap_or("")
    };

    // Step 2: Parse function signature
    let function = parse_signature(signature).unwrap();

    // Step 3: Parse arguments from JSON
    let json_args: Vec<Value> = match serde_json::from_str(args_json) {
        Ok(v) => v,
        Err(_) => return empty_cstring(),
    };

    // Step 4: Convert to Tokens
    let tokens: Vec<Token> = match json_args.iter().zip(function.inputs.iter()).map(|(value, param)| {
        parse_token(value, &param.kind)
    }).collect::<Result<_, _>>() {
        Ok(t) => t,
        Err(_) => return empty_cstring(),
    };

    // Step 5: Encode
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

fn parse_token(value: &Value, param_type: &ParamType) -> Result<Token, ()> {
    use ethabi::ParamType::*;
    match (value, param_type) {
        // address
        (Value::String(s), Address) => {
            let clean = s.trim_start_matches("0x");
            let bytes = hex::decode(clean).map_err(|_| ())?;
            if bytes.len() != 20 {
                return Err(());
            }
            Ok(Token::Address(ethabi::Address::from_slice(&bytes)))
        }

        // string
        (Value::String(s), String) => Ok(Token::String(s.clone())),

        // uint / int
        (Value::Number(n), Uint(_)) => {
            let u = n.as_u64().ok_or(())?;
            Ok(Token::Uint(u.into()))
        }
        (Value::Number(n), Int(_)) => {
            let i = n.as_i64().ok_or(())?;
            Ok(Token::Int(i.into()))
        }

        // bool
        (Value::Bool(b), Bool) => Ok(Token::Bool(*b)),

        // dynamic bytes (bytes)
        (Value::String(s), Bytes) => {
            let clean = s.trim_start_matches("0x");
            let bytes = hex::decode(clean).map_err(|_| ())?;
            Ok(Token::Bytes(bytes))
        }

        // fixed bytes (bytes32, bytes4, etc.)
        (Value::String(s), FixedBytes(size)) => {
            let clean = s.trim_start_matches("0x");
            let mut bytes = hex::decode(clean).map_err(|_| ())?;
            if bytes.len() > *size {
                bytes.truncate(*size); // cut extra bytes
            } else if bytes.len() < *size {
                bytes.resize(*size, 0u8); // pad with zeros
            }
            Ok(Token::FixedBytes(bytes))
        }

        // arrays
        (Value::Array(values), Array(inner)) => {
            let tokens = values
                .iter()
                .map(|v| parse_token(v, inner))
                .collect::<Result<Vec<_>, _>>()?;
            Ok(Token::Array(tokens))
        }

        // fixed arrays
        (Value::Array(values), FixedArray(inner, size)) => {
            let tokens = values
                .iter()
                .map(|v| parse_token(v, inner))
                .collect::<Result<Vec<_>, _>>()?;
            Ok(Token::FixedArray(tokens))
        }

        // tuples
        (Value::Array(values), Tuple(types)) => {
            if values.len() != types.len() {
                return Err(());
            }
            let tokens = values
                .iter()
                .zip(types)
                .map(|(v, t)| parse_token(v, t))
                .collect::<Result<Vec<_>, _>>()?;
            Ok(Token::Tuple(tokens))
        }

        _ => Err(()),
    }
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
    let open_paren = signature.find('(').ok_or(())?;
    let close_paren = signature.find(')').ok_or(())?;

    let name = &signature[..open_paren];
    let args_str = &signature[open_paren + 1..close_paren];
    let arg_types: Vec<ParamType> = if args_str.trim().is_empty() {
        vec![]
    } else {
        args_str.split(',')
            .map(|s| parse_param_type(s.trim()))
            .collect::<Result<_, _>>()?
    };

    let inputs = arg_types
        .into_iter()
        .enumerate()
        .map(|(i, kind)| Param {
            name: format!("arg{}", i),
            kind,
            internal_type: None
        })
        .collect();

    Ok(Function {
        name: name.to_string(),
        inputs,
        outputs: vec![],
        constant: true,
        state_mutability: StateMutability::View,
    })
}

fn parse_param_type(s: &str) -> Result<ParamType, ()> {
    match s {
        "address" => Ok(ParamType::Address),
        "uint256" => Ok(ParamType::Uint(256)),
        "uint8" => Ok(ParamType::Uint(8)),
        "bool" => Ok(ParamType::Bool),
        "string" => Ok(ParamType::String),
        "bytes" => Ok(ParamType::Bytes),
        other => {
            if other.starts_with("uint") {
                let size = other.trim_start_matches("uint").parse::<usize>().map_err(|_| ())?;
                Ok(ParamType::Uint(size))
            } else if other.starts_with("int") {
                let size = other.trim_start_matches("int").parse::<usize>().map_err(|_| ())?;
                Ok(ParamType::Int(size))
            } else {
                Err(())
            }
        }
    }
}
