use ethabi::{Address, Function, Token};
use serde_json::Value;
use hex::FromHex;
use std::ffi::{CStr, CString};
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
