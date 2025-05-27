# Sequence Unreal <> Rust Bridge

We bridge Rust code into C++ to leverage an encoding and decoding library.

The `libethabi_bridge.a` files is about 18MB large.

`EthAbiBridge.h` accesses the Rust functions and converts value types.

## Installation

Run the installation steps from the official website:

https://doc.rust-lang.org/cargo/getting-started/installation.html

## Updating Library

Navigate to the `SequencePlugin/Source/SequencePlugin/Public/EthAbi/` directory in your terminal and run the following code to compile:

```shell
cargo build --release
```

Now, take the `libethabi_bridge.a` file from the `./target/release/` directory and move it to `EthAbi/`

## Decoding:

Returns JSON array as a string of unnamed values. This array mirrors the ABI's output array. E.g. [5000] or [[0,10000,1747809900,1905662700,"0x0000000000000000000000000000000000000000000000000000000000000000"]]
