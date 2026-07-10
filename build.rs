use std::env;
use std::path::{Path, PathBuf};

fn main() {
    // --- 0. docs.rs builds documentation only (no linking). Skip the native
    //        build so doc builds succeed without CMake/libudev/network. ---
    if env::var("DOCS_RS").is_ok() {
        println!("cargo:warning=DOCS_RS set: skipping native libneoradio2 build");
        return;
    }

    // --- 1. Optionally regenerate the committed FFI (needs libclang) ---
    #[cfg(feature = "bindgen")]
    regenerate_bindings();

    // --- 2. Build the C/C++ library via its CMakeLists ---
    let mut config = cmake::Config::new("libneoradio2");
    config
        .build_target("libneoradio2")
        .define("BUILD_SHARED_LIBS", "OFF");
    // Rust on MSVC always links the non-debug dynamic CRT (`msvcrt`), even for
    // debug builds. The `cmake` crate otherwise builds the C library in Debug
    // config against the *debug* CRT (MSVCRTD), which pulls in `_CrtDbgReport`
    // and fails to link into the Rust binary. Force the matching runtime.
    if cfg!(target_env = "msvc") {
        config.define("CMAKE_MSVC_RUNTIME_LIBRARY", "MultiThreadedDLL");
    }
    let dst = config.build();
    let build_dir = dst.join("build");

    // --- 3. Find and link the static archives the CMake build produced ---
    let (lib_ext, is_msvc) = if cfg!(target_env = "msvc") {
        ("lib", true)
    } else {
        ("a", false)
    };
    // Collect the link names first instead of emitting `rustc-link-lib`
    // directives as each archive is discovered: filesystem walk order is
    // arbitrary, but on GNU ld (Linux) link order matters and `libneoradio2`
    // (which references hidapi symbols) must be emitted before `hidapi`, or
    // the link fails with unresolved symbols. Windows/MSVC is order-insensitive,
    // but we sort unconditionally so the behavior is deterministic everywhere.
    let mut link_names: Vec<String> = Vec::new();
    for entry in walk(&build_dir) {
        let name = entry.file_name().unwrap().to_string_lossy().to_string();
        // Strip exactly ONE `lib` prefix, not greedily: the CMake target is
        // named `libneoradio2`, so on Unix the archive is `lib` + `libneoradio2`
        // = `liblibneoradio2.a`. A greedy `trim_start_matches("lib")` would strip
        // both and yield `neoradio2`, so rustc would look for `libneoradio2.a`
        // and miss the actual `liblibneoradio2.a`. (MSVC keeps the exact target
        // name, `libneoradio2.lib`, and is handled by the `is_msvc` branch below.)
        let stem = name
            .strip_prefix("lib")
            .unwrap_or(name.as_str())
            .trim_end_matches(&format!(".{lib_ext}"));
        let is_static = name.ends_with(&format!(".{lib_ext}"))
            && (name.contains("neoradio2") || name.contains("hidapi"));
        if is_static {
            let dir = entry.parent().unwrap();
            println!("cargo:rustc-link-search=native={}", dir.display());
            let link_name = if is_msvc {
                name.trim_end_matches(".lib")
            } else {
                stem
            };
            link_names.push(link_name.to_string());
        }
    }
    assert!(
        !link_names.is_empty(),
        "no static libneoradio2/hidapi archive found under {}",
        build_dir.display()
    );
    // `neoradio2` first, then everything else (`hidapi`), in stable order.
    link_names.sort_by_key(|name| !name.contains("neoradio2"));
    for link_name in &link_names {
        println!("cargo:rustc-link-lib=static={link_name}");
    }

    // --- 4. C++ runtime + hidapi's platform libraries ---
    let target_os = env::var("CARGO_CFG_TARGET_OS").unwrap();
    match target_os.as_str() {
        "linux" => {
            println!("cargo:rustc-link-lib=dylib=stdc++");
            println!("cargo:rustc-link-lib=dylib=udev");
        }
        "macos" => {
            println!("cargo:rustc-link-lib=dylib=c++");
            println!("cargo:rustc-link-lib=framework=IOKit");
            println!("cargo:rustc-link-lib=framework=CoreFoundation");
            println!("cargo:rustc-link-lib=framework=AppKit");
        }
        "windows" => {
            println!("cargo:rustc-link-lib=dylib=setupapi");
            // MSVC C++ runtime is linked automatically.
        }
        other => panic!("unsupported target_os: {other}"),
    }

    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed=libneoradio2");
}

/// Recursively list files under `dir`.
fn walk(dir: &Path) -> Vec<PathBuf> {
    let mut out = Vec::new();
    if let Ok(rd) = std::fs::read_dir(dir) {
        for e in rd.flatten() {
            let p = e.path();
            if p.is_dir() {
                out.extend(walk(&p));
            } else {
                out.push(p);
            }
        }
    }
    out
}

#[cfg(feature = "bindgen")]
fn regenerate_bindings() {
    let inc = Path::new("libneoradio2/include");
    let fd = Path::new("libneoradio2/neoRAD-IO2-FrameDescription");
    let hid = Path::new("libneoradio2/hidapi/hidapi");
    let bindings = bindgen::Builder::default()
        // Put the lint-allow header at the very top of the generated file so it
        // compiles cleanly; drop bindgen's own "automatically generated" banner
        // for a stable header. Note: bindgen output is environment-dependent
        // (C enum signedness and struct-packing differ across MSVC vs GCC/clang,
        // and across libclang versions), so the committed `src/ffi.rs` is not
        // expected to be byte-identical everywhere — the CI job regenerates and
        // recompiles rather than diffing.
        .disable_header_comment()
        .raw_line(
            "#![allow(non_camel_case_types, non_snake_case, non_upper_case_globals, dead_code, clippy::all)]",
        )
        .header("libneoradio2/include/libneoradio2.h")
        .clang_arg(format!("-I{}", inc.display()))
        .clang_arg(format!("-I{}", fd.display()))
        .clang_arg(format!("-I{}", hid.display()))
        .allowlist_function("neoradio2_.*")
        .allowlist_type("[Nn]eoRADIO2.*")
        .allowlist_type("Neoradio2.*")
        .allowlist_type("CommandStatus|CommandStateType|StatusType")
        .allowlist_var("NEORADIO2_.*")
        .layout_tests(false)
        .generate()
        .expect("bindgen failed");
    bindings
        .write_to_file("src/ffi.rs")
        .expect("write src/ffi.rs");
}
