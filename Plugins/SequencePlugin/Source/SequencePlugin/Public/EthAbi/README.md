# Sequence Unreal <> Rust Bridge

We bridge Rust code into C++ to leverage an encoding and decoding library.

The `libethabi_bridge.a` files is about 18MB large.

## Installation

Run this command to install cargo:

```shell
curl https://sh.rustup.rs -sSf | sh
```

## Updating Library

Navigate to the `SequencePlugin/Source/SequencePlugin/Public/EthAbi/` directory in your terminal and run the following code to compile:

```shell
cargo build --release
```

Now, take the `libethabi_bridge.a` file from the `./target/release/` directory and move it to `EthAbi/`
