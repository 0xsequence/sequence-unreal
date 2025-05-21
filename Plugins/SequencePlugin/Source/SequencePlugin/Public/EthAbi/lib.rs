use ethabi::{Address, Function, Token, ParamType};
use serde_json::{json, Value};
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
    let abi_json = unsafe {
        if abi_json.is_null() {
            return CString::new("").unwrap().into_raw();
        }
        CStr::from_ptr(abi_json).to_str().unwrap_or("")
    };

    let result_hex = unsafe {
        if result_hex.is_null() {
            return CString::new("").unwrap().into_raw();
        }
        CStr::from_ptr(result_hex).to_str().unwrap_or("")
    };

    let result = decode_result_to_array_impl(abi_json, result_hex);

    match result {
        Ok(arr) => {
            let json_string = serde_json::to_string(&arr).unwrap_or("[]".to_string());
            CString::new(json_string).unwrap().into_raw()
        }
        Err(_) => CString::new("[]").unwrap().into_raw(),
    }
}

#[no_mangle]
pub extern "C" fn free_string(s: *mut c_char) {
    if !s.is_null() {
        unsafe {
            CString::from_raw(s);
        }
    }
}

pub fn decode_result_to_array_impl(abi_json: &str, result_hex: &str) -> Result<Vec<Value>, String> {
    // Step 1: Parse ABI JSON into ethabi::Function
    let function: Function = serde_json::from_str(abi_json)
        .map_err(|e| format!("Invalid ABI JSON: {}", e))?;

    // Step 2: Decode the result hex string into raw bytes
    let result_bytes = decode(result_hex.strip_prefix("0x").unwrap_or(result_hex))
        .map_err(|e| format!("Invalid hex input: {}", e))?;

    // Step 3: Decode the output using ethabi
    let decoded = function.decode_output(&result_bytes)
        .map_err(|e| format!("Failed to decode output: {}", e))?;

    // Step 4: Convert each Token into a serde_json::Value
    let values = decoded
        .iter()
        .zip(function.outputs.iter())
        .map(|(token, param)| token_to_json(token, &param.kind))
        .collect();

    Ok(values)
}

fn token_to_json(token: &Token, param_type: &ParamType) -> Value {
    match (token, param_type) {
        (Token::Uint(n), ParamType::Uint(_)) |
        (Token::Int(n), ParamType::Int(_)) => {
            let as_str = n.to_string();
            match as_str.parse::<u64>() {
                Ok(u) => json!(u),
                Err(_) => json!(as_str),
            }
        }
        (Token::Address(addr), ParamType::Address) => {
            json!(format!("0x{}", hex::encode(addr)))
        }
        (Token::Bool(b), ParamType::Bool) => json!(*b),
        (Token::String(s), ParamType::String) => json!(s),
        (Token::Bytes(ref b), ParamType::Bytes) |
        (Token::FixedBytes(ref b), ParamType::FixedBytes(_)) => {
            json!(format!("0x{}", hex::encode(b)))
        },
        (Token::Array(arr), ParamType::Array(inner)) |
        (Token::FixedArray(arr), ParamType::FixedArray(inner, _)) => {
            Value::Array(arr.iter().map(|t| token_to_json(t, inner)).collect())
        }
        (Token::Tuple(tokens), ParamType::Tuple(types)) => {
            let items: Vec<Value> = tokens.iter().zip(types.iter())
                .map(|(t, ty)| token_to_json(t, ty)).collect();
            json!(items)
        }
        _ => json!(null),
    }
}
