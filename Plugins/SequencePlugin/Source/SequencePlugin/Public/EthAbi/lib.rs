use ethabi::{Address, Function, Token, Param, ParamType, StateMutability};
use serde_json::{json, Map, Value};
use hex::FromHex;
use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use hex::decode;
use std::collections::HashMap;

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

fn parse_token(value: &Value, param_type: &ParamType) -> Result<Token, ()> {
    use ethabi::ParamType::*;
    match (value, param_type) {
        (Value::String(s), Address) => {
            let clean = s.trim_start_matches("0x");
            let bytes = hex::decode(clean).map_err(|_| ())?;
            Ok(Token::Address(ethabi::Address::from_slice(&bytes)))
        }
        (Value::String(s), String) => Ok(Token::String(s.clone())),
        (Value::Number(n), Uint(_)) => {
            let u = n.as_u64().ok_or(())?;
            Ok(Token::Uint(u.into()))
        }
        (Value::Number(n), Int(_)) => {
            let i = n.as_i64().ok_or(())?;
            Ok(Token::Int(i.into()))
        }
        (Value::Bool(b), Bool) => Ok(Token::Bool(*b)),
        _ => Err(()),
    }
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
