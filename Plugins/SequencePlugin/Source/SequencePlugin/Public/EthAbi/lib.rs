use ethabi::{Address, Function, Token, ParamType};
use serde_json::{json, Value};
use hex::FromHex;
use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use hex::decode;
use std::collections::HashMap;
use std::ffi::{CString, CStr};
use std::os::raw::c_char;

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
pub extern "C" fn decode_function_result(abi_json: *const c_char, encoded_data: *const c_char) -> *mut c_char {
    let abi_json = unsafe {
        if abi_json.is_null() {
            return CString::new("").unwrap().into_raw();
        }
        CStr::from_ptr(abi_json).to_str().unwrap_or("")
    };
    let decoded_input = unsafe {
        if decoded_input.is_null() {
            return CString::new("").unwrap().into_raw();
        }
        CStr::from_ptr(decoded_input).to_str().unwrap_or("")
    };

    let result = decode_function_result_impl(abi_json, decoded_input);

    match result {
        Ok(val) => {
            let json_string = val.to_string();
            CString::new(json_string).unwrap().into_raw()
        }
        Err(_) => {
            CString::new("").unwrap().into_raw()
        }
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

pub fn decode_function_result_impl(abi_json: &str, decoded_input: &str) -> Result<Value, String> {
    let function: Function = serde_json::from_str(abi_json)
        .map_err(|e| format!("Failed to parse ABI: {}", e))?;

    let input_bytes = decode(decoded_input.trim_start_matches("0x"))
        .map_err(|e| format!("Invalid hex input: {}", e))?;

    let decoded_tokens = function.decode_output(&input_bytes)
        .map_err(|e| format!("Failed to decode output: {}", e))?;

    let mut result = serde_json::Map::new();

    for (i, param) in function.outputs.iter().enumerate() {
        let name = if param.name.is_empty() {
            format!("output_{}", i)
        } else {
            param.name.clone()
        };

        let token = &decoded_tokens[i];
        let value = token_to_json(token, &param.kind);

        result.insert(name, value);
    }

    Ok(Value::Object(result))
}

fn token_to_json(token: &Token, param_type: &ParamType) -> Value {
    match (token, param_type) {
        (Token::Uint(n), ParamType::Uint(_)) |
        (Token::Int(n), ParamType::Int(_)) => {
            match n.try_to_u64() {
                Some(u) => json!(u),
                None => json!(n.to_string()),
            }
        }
        (Token::Address(addr), ParamType::Address) => {
            json!(format!("0x{}", hex::encode(addr)))
        }
        (Token::Bytes(bytes), ParamType::Bytes) |
        (Token::FixedBytes(bytes, _)) => {
            json!(format!("0x{}", hex::encode(bytes)))
        }
        (Token::Bool(b), ParamType::Bool) => {
            json!(*b)
        }
        (Token::String(s), ParamType::String) => {
            json!(s)
        }
        (Token::Array(arr), ParamType::Array(inner_type)) |
        (Token::FixedArray(arr, inner_type)) => {
            let elements: Vec<Value> = arr.iter()
                .map(|t| token_to_json(t, inner_type))
                .collect();
            json!(elements)
        }
        (Token::Tuple(inner_tokens), ParamType::Tuple(inner_types)) => {
            let fields: Vec<Value> = inner_tokens.iter().zip(inner_types.iter())
                .map(|(t, ty)| token_to_json(t, ty))
                .collect();
            json!(fields)
        }
        _ => json!(null),
    }
}
