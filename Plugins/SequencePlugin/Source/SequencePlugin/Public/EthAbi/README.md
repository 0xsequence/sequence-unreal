# Sequence Unreal <> Rust Bridge

We bridge Rust code into C++ to leverage an encoding and decoding library.

The `libethabi_bridge.a` files is about 18MB large.

`EthAbiBridge.h` accesses the Rust functions and converts value types.

## Installation

Run the installation steps from the official website:

https://doc.rust-lang.org/cargo/getting-started/installation.html

## Compiling Library

Navigate to the `SequencePlugin/Source/SequencePlugin/Public/EthAbi/` directory in your terminal and run the following code to compile:

```shell
cargo build --release
```

Now, take the `libethabi_bridge.a` file from the `./target/release/` directory and move it to `EthAbi/`

## Platforms Builds

MacOS, iOS and Android files can be build on MacOS and Windows. Windows' .lib file can only be installed on a Windows machine.

### MacOS

```shell
cargo build --release --target x86_64-apple-darwin
cargo build --release --target aarch64-apple-darwin
```

```shell
lipo -create \
  ./target/x86_64-apple-darwin/release/libethabi_bridge.a \
  ./target/aarch64-apple-darwin/release/libethabi_bridge.a \
  -output ./macos/libethabi_bridge.a
```

### Android

```shell
cargo ndk -t arm64-v8a -t armeabi-v7a -t x86_64 build --release
```

### iOS

```shell
cargo lipo --release
```

### Windows

Building .lib files only works on Windows machines.
Run the following install commands on Windows.

```shell
winget install --id Rustlang.Rustup -e --source winget
rustup default stable-x86_64-pc-windows-msvc
```

Build command:

```shell
cargo build --release --target x86_64-pc-windows-msvc
```

## Decoding:

Returns JSON array as a string of unnamed values. This array mirrors the ABI's output array. E.g. [5000] or [[0,10000,1747809900,1905662700,"0x0000000000000000000000000000000000000000000000000000000000000000"]]
