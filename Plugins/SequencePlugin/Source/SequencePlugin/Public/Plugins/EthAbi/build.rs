fn main() {
    cxx_build::bridge("lib.rs")  // returns a cc::Build
        .file("output.cc")
        .std("c++11")
        .compile("cxxbridge-demo");

    println!("cargo:rerun-if-changed=src/main.rs");
    println!("cargo:rerun-if-changed=src/demo.cc");
    println!("cargo:rerun-if-changed=include/demo.h");
}