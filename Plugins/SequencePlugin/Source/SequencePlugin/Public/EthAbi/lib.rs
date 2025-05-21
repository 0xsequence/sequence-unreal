use ethabi::{Address, Function, Token, Param, ParamType};
use serde_json::{json, Map, Value};
use hex::FromHex;
use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use hex::decode;
use std::collections::HashMap;

#[no_mangle]
pub extern "C" fn encode_function_call(abi_json: *const c_char, function_name: *const c_char, args_json: *const c_char) -> *mut c_char {
    let abi_json = unsafe { CStr::from_ptr(abi_json).to_str().unwrap() };
    let function_name = unsafe { CStr::from_ptr(function_name).to_str().unwrap() };
    let args_json = unsafe { CStr::from_ptr(args_json).to_str().unwrap() };
        
    let abi: ethabi::Contract = serde_json::from_str(abi_json).unwrap();
    let function = abi.function(function_name).unwrap();
        
    let args: Vec<Value> = serde_json::from_str(args_json).unwrap();
            
    let tokens_result: Result<Vec<Token>, &'static str> = args.into_iter().map(|arg| {
        match arg {
            Value::String(s) => {
                if s.starts_with("0x") && s.len() == 42 {
                    let addr = s.trim_start_matches("0x");
                    match Vec::from_hex(addr) {
                        Ok(bytes) if bytes.len() == 20 => Ok(Token::Address(Address::from_slice(&bytes))),
                        _ => Err("Invalid address hex format"),
                    }
                } else {
                    Err("Unexpected string format")
                }
            }
            Value::Number(num) => {
                if let Some(u) = num.as_u64() {
                    Ok(Token::Uint(u.into()))
                } else {
                    Err("Number too big or invalid")
                }
            }
            _ => Err("Unsupported argument type"),
        }
    }).collect();
    
    let tokens = match tokens_result {
        Ok(t) => t,
        Err(_) => {
            let empty = CString::new("").unwrap();
            return empty.into_raw();
        }
    };
            
    let encoded = match function.encode_input(&tokens) {
        Ok(e) => e,
        Err(_) => {
            let empty = CString::new("").unwrap();
            return empty.into_raw();
        }
    };
        
    let hex = hex::encode(encoded);
    CString::new("hex").unwrap().into_raw()
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
pub extern "C" fn free_string(s: *mut c_char) {
    if !s.is_null() {
        unsafe {
            CString::from_raw(s);
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

fn token_to_json(token: &Token, param_type: &ParamType) -> Value {
    match (token, param_type) {
        (Token::Uint(n), ParamType::Uint(_)) |
        (Token::Int(n), ParamType::Int(_)) => {
            let as_str = n.to_string();
            match as_str.parse::<u64>() {
                Ok(u) => Value::from(u),
                Err(_) => Value::String(as_str),
            }
        }
        (Token::Address(addr), ParamType::Address) => {
            Value::String(format!("0x{}", hex::encode(addr)))
        }
        (Token::Bool(b), ParamType::Bool) => Value::Bool(*b),
        (Token::String(s), ParamType::String) => Value::String(s.clone()),
        (Token::Bytes(ref b), ParamType::Bytes) |
        (Token::FixedBytes(ref b), ParamType::FixedBytes(_)) => {
            Value::String(format!("0x{}", hex::encode(b)))
        }
        (Token::Array(arr), ParamType::Array(inner)) => {
            Value::Array(arr.iter().map(|t| token_to_json(t, inner)).collect())
        }
        (Token::FixedArray(arr), ParamType::FixedArray(inner, _)) => {
            Value::Array(arr.iter().map(|t| token_to_json(t, inner)).collect())
        }
        (Token::Tuple(tokens), ParamType::Tuple(types)) => {
            let elements: Vec<Value> = tokens.iter()
                .zip(types.iter())
                .map(|(t, ty)| token_to_json(t, ty))
                .collect();
            Value::Array(elements)
        }
        _ => Value::Null,
    }
}
