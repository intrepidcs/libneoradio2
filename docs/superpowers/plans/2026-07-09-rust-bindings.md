# Rust Bindings Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** A safe, idiomatic Rust crate `neoradio2` (Cargo.toml at repo root) that builds the existing libneoradio2 C/C++ library from source and wraps its full C API.

**Architecture:** Single crate. `build.rs` builds `libneoradio2/CMakeLists.txt` via the `cmake` crate and links the static archives + C++ runtime + hidapi's system libs. A committed bindgen layer (`src/ffi.rs`) provides raw FFI; safe modules wrap it with `Device`, `Result<T, Error>`, `Drop`-close, and Rust enums. Settings/calibration/statistics use the `repr(C)` structs re-exported under `neoradio2::raw`.

**Tech Stack:** Rust 2021 (cargo 1.96 present), `cmake` build-dep, `bindgen` (opt-in build-dep), the existing CMake/hidapi C build.

## Global Constraints

- Package name `neoradio2`, `Cargo.toml` at repo root `C:\dev\libneoradio2`. `edition = "2021"`. License `MIT`.
- `cargo build` must need only CMake + a C/C++ toolchain — **no libclang** by default. Regeneration of `src/ffi.rs` is behind the `bindgen` cargo feature.
- Submodules `hidapi` and `neoRAD-IO2-FrameDescription` must be checked out before building.
- C API is `libneoradio2/include/libneoradio2.h` + `libneoradio2common.h`. Handle: `#define neoradio2_handle long`. Return codes: `NEORADIO2_SUCCESS=0`, `NEORADIO2_FAILURE=1`, `NEORADIO2_ERR_WBLOCK=2`, `NEORADIO2_ERR_INPROGRESS=3`, `NEORADIO2_ERR_FAILURE=4`. `NEORADIO2_MAX_DEVS=8`.
- The library is C++ — every link config must pull in the C++ runtime (`stdc++` on GNU/Linux, `c++` on macOS, auto on MSVC).
- Do not modify the existing C/CMake/Python build files except `.gitignore`.

## File structure

- `Cargo.toml` — crate manifest (root).
- `.gitignore` — add `/target` and `Cargo.lock` is kept (bin? no, lib → ignore Cargo.lock is optional; keep it out).
- `build.rs` — CMake build + link directives; opt-in bindgen regen.
- `src/ffi.rs` — committed bindgen output (raw).
- `src/lib.rs` — crate root; module wiring, `pub use`, `raw` module, crate docs.
- `src/error.rs` — `Error`, `Result<T>`, `check()`.
- `src/types.rs` — `DeviceType`, `CalType`, `LedMode`, `StatusType`, `CommandStatus` enums + conversions, and `set_blocking`/`is_blocking`.
- `src/device.rs` — `DeviceInfo`, `Device`, all handle methods.
- `src/settings.rs` — settings + statistics + status methods (impl blocks on `Device`).
- `src/calibration.rs` — calibration methods (impl block on `Device`).
- `examples/find_and_read.rs` — runnable example.
- `tests/api.rs` — no-hardware unit tests.
- `tests/hardware.rs` — adaptive hardware tests (skip if nothing connected).
- `.github/workflows/rust.yml` — CI.

---

### Task 1: Crate skeleton, CMake build, committed FFI, link smoke test

**Files:**
- Create: `Cargo.toml`, `build.rs`, `src/lib.rs`, `src/ffi.rs`
- Modify: `.gitignore`
- Test: `tests/link.rs`

**Interfaces:**
- Produces: crate builds and links against the static C library; `neoradio2::ffi` raw module with all `neoradio2_*` fns, `Neoradio2DeviceInfo`, `neoradio2_handle`, `NEORADIO2_*` consts, and the frame-description structs (`neoRADIO2_settings`, `neoRADIO2frame_calHeader`, `neoRADIO2_PerfStatistics`, `neoRADIO2AOUT_header`, `neoRADIO2CalType`, `neoRADIO2_LEDMode`, `neoRADIO2_deviceTypes`).

- [ ] **Step 1: Create `Cargo.toml`**

```toml
[package]
name = "neoradio2"
version = "1.4.1"
edition = "2021"
rust-version = "1.70"
license = "MIT"
description = "Rust bindings for Intrepid Control Systems neoRAD-IO2 devices"
repository = "https://github.com/intrepidcs/libneoradio2"
readme = "python/README.md"
build = "build.rs"
exclude = ["python/*", "doc/*", "libneoradio2/hidapi/tests/*"]

[features]
# Regenerate src/ffi.rs at build time (needs libclang). Off by default.
bindgen = ["dep:bindgen"]

[build-dependencies]
cmake = "0.1"
bindgen = { version = "0.70", optional = true }
```

- [ ] **Step 2: Add `/target` to `.gitignore`**

Append these lines to `.gitignore`:

```
# Rust
/target
```

- [ ] **Step 3: Write `build.rs`**

```rust
use std::env;
use std::path::{Path, PathBuf};

fn main() {
    // --- 1. Optionally regenerate the committed FFI (needs libclang) ---
    #[cfg(feature = "bindgen")]
    regenerate_bindings();

    // --- 2. Build the C/C++ library via its CMakeLists ---
    let dst = cmake::Config::new("libneoradio2")
        .build_target("libneoradio2")
        .define("BUILD_SHARED_LIBS", "OFF")
        .build();
    let build_dir = dst.join("build");

    // --- 3. Find and link the static archives the CMake build produced ---
    let (lib_ext, is_msvc) = if cfg!(target_env = "msvc") { ("lib", true) } else { ("a", false) };
    let mut linked = false;
    for entry in walk(&build_dir) {
        let name = entry.file_name().unwrap().to_string_lossy().to_string();
        let stem = name.trim_start_matches("lib").trim_end_matches(&format!(".{lib_ext}"));
        let is_static = name.ends_with(&format!(".{lib_ext}"))
            && (name.contains("neoradio2") || name.contains("hidapi"));
        if is_static {
            let dir = entry.parent().unwrap();
            println!("cargo:rustc-link-search=native={}", dir.display());
            let link_name = if is_msvc { name.trim_end_matches(".lib") } else { stem };
            println!("cargo:rustc-link-lib=static={link_name}");
            linked = true;
        }
    }
    assert!(linked, "no static libneoradio2/hidapi archive found under {}", build_dir.display());

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
            if p.is_dir() { out.extend(walk(&p)); } else { out.push(p); }
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
    bindings.write_to_file("src/ffi.rs").expect("write src/ffi.rs");
}
```

- [ ] **Step 4: Generate `src/ffi.rs` once (maintainer step, uses installed LLVM/libclang)**

Run (Windows, LLVM already installed):

```bash
cargo install bindgen-cli --version ^0.70 || true
LIBCLANG_PATH="C:/Program Files/LLVM/bin" cargo build --features bindgen
```

Expected: `src/ffi.rs` is created/overwritten with the raw bindings. Verify it contains `pub fn neoradio2_open`, `pub struct Neoradio2DeviceInfo`, `pub struct neoRADIO2_settings`, and `pub const NEORADIO2_MAX_DEVS`.

If `cargo build --features bindgen` cannot find libclang, set `LIBCLANG_PATH` to the directory containing `libclang.dll`/`.so`/`.dylib` (on this machine: `C:/Program Files/LLVM/bin`).

- [ ] **Step 5: Prepend an allow-attributes header to `src/ffi.rs`**

Insert at the very top of `src/ffi.rs`:

```rust
#![allow(non_camel_case_types, non_snake_case, non_upper_case_globals, dead_code)]
```

- [ ] **Step 6: Write `src/lib.rs` (minimal, exposes ffi)**

```rust
//! Rust bindings for the Intrepid Control Systems neoRAD-IO2 device library.

pub mod ffi;
```

- [ ] **Step 7: Write the link smoke test `tests/link.rs`**

```rust
// Proves the C library links: is_blocking() returns an int without crashing.
#[test]
fn library_links_and_calls() {
    let blocking = unsafe { neoradio2::ffi::neoradio2_is_blocking() };
    assert!(blocking == 0 || blocking == 1, "unexpected value: {blocking}");
}
```

- [ ] **Step 8: Run the smoke test**

Run: `cargo test --test link -- --nocapture`
Expected: compiles, links the static C library, PASS.

- [ ] **Step 9: Commit**

```bash
git add Cargo.toml build.rs src/ffi.rs src/lib.rs tests/link.rs .gitignore
git commit -m "feat(rust): crate skeleton, cmake build, committed FFI, link smoke test"
```

---

### Task 2: Error type and return-code mapping

**Files:**
- Create: `src/error.rs`
- Modify: `src/lib.rs`
- Test: in `src/error.rs` (`#[cfg(test)]`)

**Interfaces:**
- Produces: `neoradio2::Error` (enum), `neoradio2::Result<T>`, and `pub(crate) fn check(code: c_int) -> Result<()>`.

- [ ] **Step 1: Write the failing test in `src/error.rs`**

```rust
#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn maps_return_codes() {
        assert!(check(0).is_ok());
        assert_eq!(check(1).unwrap_err(), Error::Failure);
        assert_eq!(check(2).unwrap_err(), Error::WouldBlock);
        assert_eq!(check(3).unwrap_err(), Error::InProgress);
        assert_eq!(check(4).unwrap_err(), Error::Failure);
        assert_eq!(check(99).unwrap_err(), Error::Failure);
    }
}
```

- [ ] **Step 2: Write `src/error.rs` implementation (above the test module)**

```rust
use std::os::raw::c_int;

/// Errors returned by the neoRAD-IO2 API.
#[derive(Debug, Clone, PartialEq, Eq)]
#[non_exhaustive]
pub enum Error {
    /// General failure (`NEORADIO2_FAILURE` / `NEORADIO2_ERR_FAILURE`).
    Failure,
    /// Non-blocking call would block (`NEORADIO2_ERR_WBLOCK`).
    WouldBlock,
    /// Non-blocking operation still in progress (`NEORADIO2_ERR_INPROGRESS`).
    InProgress,
    /// A Rust string contained an interior NUL byte.
    NulByte,
}

impl std::fmt::Display for Error {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let s = match self {
            Error::Failure => "neoRAD-IO2 operation failed",
            Error::WouldBlock => "operation would block (non-blocking mode)",
            Error::InProgress => "operation in progress (non-blocking mode)",
            Error::NulByte => "string contained an interior NUL byte",
        };
        f.write_str(s)
    }
}

impl std::error::Error for Error {}

/// Crate result type.
pub type Result<T> = std::result::Result<T, Error>;

/// Map a C return code to a `Result`.
pub(crate) fn check(code: c_int) -> Result<()> {
    match code {
        0 => Ok(()),
        2 => Err(Error::WouldBlock),
        3 => Err(Error::InProgress),
        _ => Err(Error::Failure),
    }
}
```

- [ ] **Step 3: Wire the module in `src/lib.rs`**

```rust
//! Rust bindings for the Intrepid Control Systems neoRAD-IO2 device library.

pub mod ffi;
mod error;

pub use error::{Error, Result};
pub(crate) use error::check;
```

- [ ] **Step 4: Run tests**

Run: `cargo test error -- --nocapture`
Expected: `maps_return_codes` PASS.

- [ ] **Step 5: Commit**

```bash
git add src/error.rs src/lib.rs
git commit -m "feat(rust): Error type and return-code mapping"
```

---

### Task 3: Enums and global blocking mode (`src/types.rs`)

**Files:**
- Create: `src/types.rs`
- Modify: `src/lib.rs`
- Test: in `src/types.rs`

**Interfaces:**
- Produces: `DeviceType`, `CalType`, `LedMode`, `StatusType`, `CommandStatus` enums with `from_raw(i32) -> Option<Self>` and `as_raw(&self) -> i32`; free functions `set_blocking(bool, Duration)` and `is_blocking() -> bool`.

- [ ] **Step 1: Write the failing test in `src/types.rs`**

```rust
#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn device_type_round_trip() {
        for (v, t) in [(0, DeviceType::Tc), (3, DeviceType::Ain), (6, DeviceType::Badge)] {
            assert_eq!(DeviceType::from_raw(v), Some(t));
            assert_eq!(t.as_raw(), v);
        }
        assert_eq!(DeviceType::from_raw(42), None);
    }
    #[test]
    fn caltype_values() {
        assert_eq!(CalType::Enabled.as_raw(), 0);
        assert_eq!(CalType::NoCal.as_raw(), 1);
    }
}
```

- [ ] **Step 2: Write `src/types.rs` implementation (above the test module)**

```rust
use std::time::Duration;
use crate::ffi;

macro_rules! int_enum {
    ($(#[$m:meta])* $name:ident { $($variant:ident = $val:expr),* $(,)? }) => {
        $(#[$m])*
        #[derive(Debug, Clone, Copy, PartialEq, Eq)]
        pub enum $name { $($variant),* }
        impl $name {
            /// Convert from the C integer value.
            pub fn from_raw(v: i32) -> Option<Self> {
                match v { $($val => Some($name::$variant),)* _ => None }
            }
            /// The C integer value.
            pub fn as_raw(self) -> i32 { match self { $($name::$variant => $val),* } }
        }
    };
}

int_enum!(
    /// neoRAD-IO2 device type.
    DeviceType { Tc = 0, Dio = 1, PwrRly = 2, Ain = 3, Aout = 4, CanHub = 5, Badge = 6, Host = 0xFF }
);
int_enum!(
    /// Sensor read calibration mode.
    CalType { Enabled = 0, NoCal = 1, NoCalEnhanced = 2 }
);
int_enum!(
    /// LED toggle mode.
    LedMode { Off = 0, On = 1, BlinkOnce = 2, BlinkDurationMs = 3 }
);
int_enum!(
    /// Status query type (see `Device::status`).
    StatusType {
        Chain = 0, AppStart = 1, Pcbsn = 2, SensorRead = 3, SensorWrite = 4,
        SettingsRead = 5, SettingsWrite = 6, Calibration = 7, CalibrationPoints = 8,
        CalibrationStored = 9, CalibrationInfo = 10, LedToggle = 11,
    }
);
int_enum!(
    /// Command status returned by `Device::status`.
    CommandStatus { InProgress = 0, Finished = 1, Error = 2 }
);

/// Set process-global blocking mode. In blocking mode, calls wait up to
/// `timeout` for completion; in non-blocking mode they return immediately.
pub fn set_blocking(blocking: bool, timeout: Duration) {
    let ms = timeout.as_millis().min(i64::MAX as u128) as std::os::raw::c_longlong;
    unsafe { ffi::neoradio2_set_blocking(if blocking { 1 } else { 0 }, ms) };
}

/// Whether the API is in blocking mode.
pub fn is_blocking() -> bool {
    unsafe { ffi::neoradio2_is_blocking() != 0 }
}
```

- [ ] **Step 3: Wire the module in `src/lib.rs`**

Add to `src/lib.rs`:

```rust
mod types;
pub use types::{CalType, CommandStatus, DeviceType, LedMode, StatusType, is_blocking, set_blocking};
```

- [ ] **Step 4: Run tests**

Run: `cargo test types -- --nocapture`
Expected: `device_type_round_trip`, `caltype_values` PASS.

- [ ] **Step 5: Commit**

```bash
git add src/types.rs src/lib.rs
git commit -m "feat(rust): device/cal/led/status enums and global blocking mode"
```

---

### Task 4: `DeviceInfo`, `Device` lifecycle (find/open/close/state)

**Files:**
- Create: `src/device.rs`
- Modify: `src/lib.rs`
- Test: in `src/device.rs` + `tests/api.rs`

**Interfaces:**
- Consumes: `check` (Task 2).
- Produces: `DeviceInfo` (`name()`, `serial()`, `vendor_id()`, `product_id()`), `Device` with `find() -> Result<Vec<DeviceInfo>>`, `open(&DeviceInfo) -> Result<Device>`, `is_opened()/is_closed() -> Result<bool>`, and a `Drop` that closes. `pub(crate) handle: ffi::neoradio2_handle` accessible within crate for later method tasks. `Device: Send`, not `Sync`.

- [ ] **Step 1: Write the failing test `tests/api.rs`**

```rust
// No-hardware tests: these must pass with nothing plugged in.
#[test]
fn find_returns_a_vec_without_error() {
    // With no device connected this returns an empty Vec (never an error).
    let list = neoradio2::Device::find().expect("find should not error");
    // Length is >= 0 by construction; assert the call succeeded and is bounded.
    assert!(list.len() <= 8);
}
```

- [ ] **Step 2: Write `src/device.rs`**

```rust
use std::os::raw::{c_char, c_int, c_uint};
use crate::{check, ffi, Result};

const MAX_DEVS: usize = ffi::NEORADIO2_MAX_DEVS as usize;

fn cstr_to_string(buf: &[c_char]) -> String {
    let bytes: Vec<u8> = buf.iter().take_while(|&&c| c != 0).map(|&c| c as u8).collect();
    String::from_utf8_lossy(&bytes).into_owned()
}

/// Information about a discovered neoRAD-IO2 USB device.
#[derive(Clone)]
pub struct DeviceInfo(pub(crate) ffi::Neoradio2DeviceInfo);

impl DeviceInfo {
    /// Product name (e.g. "neoRAD-IO2-AIN").
    pub fn name(&self) -> String { cstr_to_string(&self.0.name) }
    /// Serial string (e.g. "IB0370").
    pub fn serial(&self) -> String { cstr_to_string(&self.0.serial_str) }
    /// USB vendor id.
    pub fn vendor_id(&self) -> i32 { self.0.vendor_id }
    /// USB product id.
    pub fn product_id(&self) -> i32 { self.0.product_id }
}

impl std::fmt::Debug for DeviceInfo {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("DeviceInfo")
            .field("name", &self.name())
            .field("serial", &self.serial())
            .field("vendor_id", &format_args!("{:#06x}", self.vendor_id()))
            .field("product_id", &format_args!("{:#06x}", self.product_id()))
            .finish()
    }
}

/// An open connection to a neoRAD-IO2 chain (closed automatically on drop).
///
/// `Device` is `Send` but not `Sync`: the underlying C library is not safe for
/// concurrent calls on the same handle, and blocking mode is process-global.
pub struct Device {
    pub(crate) handle: ffi::neoradio2_handle,
}

// Safe to move a handle between threads; not safe to share (&Device) across them.
unsafe impl Send for Device {}

impl Device {
    /// Enumerate attached neoRAD-IO2 USB devices.
    pub fn find() -> Result<Vec<DeviceInfo>> {
        let mut buf: [ffi::Neoradio2DeviceInfo; MAX_DEVS] =
            unsafe { std::mem::zeroed() };
        let mut count: c_uint = MAX_DEVS as c_uint;
        check(unsafe { ffi::neoradio2_find(buf.as_mut_ptr(), &mut count) })?;
        let n = (count as usize).min(MAX_DEVS);
        Ok(buf[..n].iter().map(|raw| DeviceInfo(*raw)).collect())
    }

    /// Open a device discovered by [`find`](Device::find).
    pub fn open(info: &DeviceInfo) -> Result<Device> {
        let mut raw = info.0;
        let mut handle: ffi::neoradio2_handle = -1;
        check(unsafe { ffi::neoradio2_open(&mut handle, &mut raw) })?;
        Ok(Device { handle })
    }

    /// Whether the device is currently open.
    pub fn is_opened(&self) -> Result<bool> {
        let mut out: c_int = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_is_opened(&mut h, &mut out) })?;
        Ok(out != 0)
    }

    /// Whether the device is currently closed.
    pub fn is_closed(&self) -> Result<bool> {
        let mut out: c_int = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_is_closed(&mut h, &mut out) })?;
        Ok(out != 0)
    }
}

impl Drop for Device {
    fn drop(&mut self) {
        unsafe { ffi::neoradio2_close(&mut self.handle) };
    }
}
```

- [ ] **Step 3: Wire the module in `src/lib.rs`**

Add:

```rust
mod device;
pub use device::{Device, DeviceInfo};
```

- [ ] **Step 4: Run tests**

Run: `cargo test --test api -- --nocapture`
Expected: `find_returns_a_vec_without_error` PASS (no hardware needed).

- [ ] **Step 5: Commit**

```bash
git add src/device.rs src/lib.rs tests/api.rs
git commit -m "feat(rust): DeviceInfo and Device find/open/close/state"
```

---

### Task 5: Chain, app, and bootloader methods

**Files:**
- Modify: `src/device.rs`
- Test: `tests/api.rs` (compile-only smoke; behavior covered by hardware test in Task 10)

**Interfaces:**
- Consumes: `Device.handle`, `check`.
- Produces: `Device::chain_identify()`, `chain_is_identified() -> Result<bool>`, `chain_count(identify: bool) -> Result<i32>`, `app_start(device, bank)`, `app_is_started(device, bank) -> Result<bool>`, `enter_bootloader(device, bank)`. Signatures: `device: i32, bank: i32`.

- [ ] **Step 1: Add methods to the `impl Device` block in `src/device.rs`**

```rust
impl Device {
    /// Identify the device chain.
    pub fn chain_identify(&self) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_chain_identify(&mut h) })
    }

    /// Whether the chain has been identified.
    pub fn chain_is_identified(&self) -> Result<bool> {
        let mut out: c_int = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_chain_is_identified(&mut h, &mut out) })?;
        Ok(out != 0)
    }

    /// Number of devices in the chain. `identify` re-identifies first.
    pub fn chain_count(&self, identify: bool) -> Result<i32> {
        let mut out: c_int = 0;
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_get_chain_count(&mut h, &mut out, identify as c_int)
        })?;
        Ok(out)
    }

    /// Start application firmware on `device`/`bank` (bank is a bitmask).
    pub fn app_start(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_app_start(&mut h, device, bank) })
    }

    /// Whether application firmware is running on `device`/`bank`.
    pub fn app_is_started(&self, device: i32, bank: i32) -> Result<bool> {
        let mut out: c_int = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_app_is_started(&mut h, device, bank, &mut out) })?;
        Ok(out != 0)
    }

    /// Enter the bootloader on `device`/`bank`.
    pub fn enter_bootloader(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_enter_bootloader(&mut h, device, bank) })
    }
}
```

- [ ] **Step 2: Verify it compiles**

Run: `cargo build`
Expected: builds clean.

- [ ] **Step 3: Commit**

```bash
git add src/device.rs
git commit -m "feat(rust): chain identify/count, app start, bootloader"
```

---

### Task 6: Identity getters

**Files:**
- Modify: `src/device.rs`

**Interfaces:**
- Produces on `Device`: `serial_number(device, bank) -> Result<u32>`, `manufacturer_date(device, bank) -> Result<(i32, i32, i32)>` (year, month, day), `firmware_version(device, bank) -> Result<(i32, i32)>`, `hardware_revision(device, bank) -> Result<(i32, i32)>`, `device_type(device, bank) -> Result<u32>`, `request_pcbsn(device, bank)`, `pcbsn(device, bank) -> Result<String>`.

- [ ] **Step 1: Add methods to `impl Device` in `src/device.rs`**

```rust
impl Device {
    /// Serial number (base-10) of `device`/`bank`.
    pub fn serial_number(&self, device: i32, bank: i32) -> Result<u32> {
        let mut out: c_uint = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_get_serial_number(&mut h, device, bank, &mut out) })?;
        Ok(out)
    }

    /// Manufacturing date `(year, month, day)`.
    pub fn manufacturer_date(&self, device: i32, bank: i32) -> Result<(i32, i32, i32)> {
        let (mut y, mut m, mut d) = (0, 0, 0);
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_get_manufacturer_date(&mut h, device, bank, &mut y, &mut m, &mut d)
        })?;
        Ok((y, m, d))
    }

    /// Firmware version `(major, minor)`.
    pub fn firmware_version(&self, device: i32, bank: i32) -> Result<(i32, i32)> {
        let (mut major, mut minor) = (0, 0);
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_get_firmware_version(&mut h, device, bank, &mut major, &mut minor)
        })?;
        Ok((major, minor))
    }

    /// Hardware revision `(major, minor)`.
    pub fn hardware_revision(&self, device: i32, bank: i32) -> Result<(i32, i32)> {
        let (mut major, mut minor) = (0, 0);
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_get_hardware_revision(&mut h, device, bank, &mut major, &mut minor)
        })?;
        Ok((major, minor))
    }

    /// Device type code (see [`DeviceType::from_raw`](crate::DeviceType::from_raw)).
    pub fn device_type(&self, device: i32, bank: i32) -> Result<u32> {
        let mut out: c_uint = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_get_device_type(&mut h, device, bank, &mut out) })?;
        Ok(out)
    }

    /// Request the PCB serial number (call before [`pcbsn`](Device::pcbsn)).
    pub fn request_pcbsn(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_request_pcbsn(&mut h, device, bank) })
    }

    /// PCB serial number string.
    pub fn pcbsn(&self, device: i32, bank: i32) -> Result<String> {
        let mut buf = [0 as c_char; 17];
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_get_pcbsn(&mut h, device, bank, buf.as_mut_ptr()) })?;
        Ok(cstr_to_string(&buf))
    }
}
```

- [ ] **Step 2: Verify it compiles**

Run: `cargo build`
Expected: clean.

- [ ] **Step 3: Commit**

```bash
git add src/device.rs
git commit -m "feat(rust): identity getters (serial, date, fw, hw, type, pcbsn)"
```

---

### Task 7: Sensor read/write

**Files:**
- Modify: `src/device.rs`

**Interfaces:**
- Consumes: `CalType` (Task 3).
- Produces on `Device`: `request_sensor_data(device, bank, cal: CalType)`, `read_sensor_float(device, bank) -> Result<f32>`, `read_sensor_array(device, bank) -> Result<Vec<i32>>`, `write_sensor(device, bank, data: &[u8])`, `write_sensor_successful(device, bank) -> Result<()>`.

- [ ] **Step 1: Add methods to `impl Device` in `src/device.rs`**

```rust
use crate::CalType;

impl Device {
    /// Request a fresh sensor sample for `device`/`bank`.
    pub fn request_sensor_data(&self, device: i32, bank: i32, cal: CalType) -> Result<()> {
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_request_sensor_data(&mut h, device, bank, cal.as_raw())
        })
    }

    /// Read a single sensor value as `f32`.
    pub fn read_sensor_float(&self, device: i32, bank: i32) -> Result<f32> {
        let mut out: f32 = 0.0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_read_sensor_float(&mut h, device, bank, &mut out) })?;
        Ok(out)
    }

    /// Read the raw sensor sample as an array of integers (one per byte).
    pub fn read_sensor_array(&self, device: i32, bank: i32) -> Result<Vec<i32>> {
        let mut arr = [0 as c_int; 64];
        let mut len: c_int = arr.len() as c_int;
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_read_sensor_array(&mut h, device, bank, arr.as_mut_ptr(), &mut len)
        })?;
        let n = (len.max(0) as usize).min(arr.len());
        Ok(arr[..n].to_vec())
    }

    /// Write sensor/actuator data for `device`/`bank`.
    pub fn write_sensor(&self, device: i32, bank: i32, data: &[u8]) -> Result<()> {
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_write_sensor(
                &mut h, device, bank,
                data.as_ptr() as *mut u8, data.len() as c_int,
            )
        })
    }

    /// Whether the last [`write_sensor`](Device::write_sensor) completed.
    pub fn write_sensor_successful(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_write_sensor_successful(&mut h, device, bank) })
    }
}
```

- [ ] **Step 2: Verify it compiles**

Run: `cargo build`
Expected: clean.

- [ ] **Step 3: Commit**

```bash
git add src/device.rs
git commit -m "feat(rust): sensor read (float/array) and write"
```

---

### Task 8: LED, settings, statistics, status (`src/settings.rs`)

**Files:**
- Create: `src/settings.rs`
- Modify: `src/lib.rs`

**Interfaces:**
- Consumes: `Device`, `check`, `ffi`, `StatusType`.
- Produces additional `impl Device` methods: `toggle_led(device, bank, mode: LedMode, led_enables: i32, ms: i32)`, `toggle_led_successful(device, bank)`, `request_settings(device, bank)`, `read_settings(device, bank) -> Result<raw::neoRADIO2_settings>`, `write_settings(device, bank, &raw::neoRADIO2_settings)`, `write_settings_successful(device, bank)`, `write_default_settings(device, bank)`, `request_statistics(device, bank)`, `read_statistics(device, bank) -> Result<raw::neoRADIO2_PerfStatistics>`, `status(device, bank, bitfield: bool, ty: StatusType) -> Result<CommandStatus>`.

- [ ] **Step 1: Write `src/settings.rs`**

```rust
use std::os::raw::c_int;
use crate::{check, ffi, CommandStatus, Device, LedMode, Result, StatusType};

impl Device {
    /// Toggle LEDs on `device`/`bank`.
    pub fn toggle_led(&self, device: i32, bank: i32, mode: LedMode, led_enables: i32, ms: i32)
        -> Result<()>
    {
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_toggle_led(&mut h, device, bank, mode.as_raw(), led_enables, ms)
        })
    }

    /// Whether the last [`toggle_led`](Device::toggle_led) completed.
    pub fn toggle_led_successful(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_toggle_led_successful(&mut h, device, bank) })
    }

    /// Request the device settings (call before [`read_settings`](Device::read_settings)).
    pub fn request_settings(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_request_settings(&mut h, device, bank) })
    }

    /// Read the raw device settings struct.
    pub fn read_settings(&self, device: i32, bank: i32) -> Result<ffi::neoRADIO2_settings> {
        let mut s: ffi::neoRADIO2_settings = unsafe { std::mem::zeroed() };
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_read_settings(&mut h, device, bank, &mut s) })?;
        Ok(s)
    }

    /// Write the raw device settings struct.
    pub fn write_settings(&self, device: i32, bank: i32, settings: &ffi::neoRADIO2_settings)
        -> Result<()>
    {
        let mut s = *settings;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_write_settings(&mut h, device, bank, &mut s) })
    }

    /// Whether the last [`write_settings`](Device::write_settings) completed.
    pub fn write_settings_successful(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_write_settings_successful(&mut h, device, bank) })
    }

    /// Restore default settings on `device`/`bank`.
    pub fn write_default_settings(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_write_default_settings(&mut h, device, bank) })
    }

    /// Request performance statistics.
    pub fn request_statistics(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_request_statistics(&mut h, device, bank) })
    }

    /// Read raw performance statistics.
    pub fn read_statistics(&self, device: i32, bank: i32)
        -> Result<ffi::neoRADIO2_PerfStatistics>
    {
        let mut s: ffi::neoRADIO2_PerfStatistics = unsafe { std::mem::zeroed() };
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_read_statistics(&mut h, device, bank, &mut s) })?;
        Ok(s)
    }

    /// Query a command status.
    pub fn status(&self, device: i32, bank: i32, bitfield: bool, ty: StatusType)
        -> Result<CommandStatus>
    {
        let mut out: ffi::CommandStatus = 0;
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_get_status(
                &mut h, device, bank, bitfield as c_int, ty.as_raw() as ffi::StatusType, &mut out,
            )
        })?;
        Ok(CommandStatus::from_raw(out as i32).unwrap_or(CommandStatus::Error))
    }
}
```

- [ ] **Step 2: Wire into `src/lib.rs`**

Add:

```rust
mod settings;
```

- [ ] **Step 3: Verify it compiles**

Run: `cargo build`
Expected: clean. (If `ffi::CommandStatus`/`ffi::StatusType` are generated as `type = c_int` or as enums, adjust the casts — inspect `src/ffi.rs` for their exact Rust type and match it.)

- [ ] **Step 4: Commit**

```bash
git add src/settings.rs src/lib.rs
git commit -m "feat(rust): LED, settings, statistics, status"
```

---

### Task 9: Calibration methods (`src/calibration.rs`)

**Files:**
- Create: `src/calibration.rs`
- Modify: `src/lib.rs`

**Interfaces:**
- Produces `impl Device` methods for the full calibration flow using `ffi::neoRADIO2frame_calHeader`: `request_calibration`, `read_calibration_array`, `request_calibration_points`, `read_calibration_points_array`, `write_calibration`, `write_calibration_successful`, `write_calibration_points`, `write_calibration_points_successful`, `store_calibration`, `is_calibration_stored -> Result<bool>`, `calibration_is_valid -> Result<bool>`, `clear_calibration`, `request_calibration_info`, `read_calibration_info -> Result<neoRADIO2frame_calHeader>`.

- [ ] **Step 1: Write `src/calibration.rs`**

```rust
use std::os::raw::c_int;
use crate::{check, ffi, Device, Result};

type CalHeader = ffi::neoRADIO2frame_calHeader;

impl Device {
    /// Request calibration data (fills nothing; pair with `read_calibration_array`).
    pub fn request_calibration(&self, device: i32, bank: i32, header: &mut CalHeader) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_request_calibration(&mut h, device, bank, header) })
    }

    /// Read calibration values into a `Vec<f32>`.
    pub fn read_calibration_array(&self, device: i32, bank: i32, header: &mut CalHeader)
        -> Result<Vec<f32>>
    {
        let mut arr = [0f32; 64];
        let mut len: c_int = arr.len() as c_int;
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_read_calibration_array(&mut h, device, bank, header, arr.as_mut_ptr(), &mut len)
        })?;
        let n = (len.max(0) as usize).min(arr.len());
        Ok(arr[..n].to_vec())
    }

    /// Request calibration points.
    pub fn request_calibration_points(&self, device: i32, bank: i32, header: &mut CalHeader)
        -> Result<()>
    {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_request_calibration_points(&mut h, device, bank, header) })
    }

    /// Read calibration points into a `Vec<f32>`.
    pub fn read_calibration_points_array(&self, device: i32, bank: i32, header: &mut CalHeader)
        -> Result<Vec<f32>>
    {
        let mut arr = [0f32; 64];
        let mut len: c_int = arr.len() as c_int;
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_read_calibration_points_array(&mut h, device, bank, header, arr.as_mut_ptr(), &mut len)
        })?;
        let n = (len.max(0) as usize).min(arr.len());
        Ok(arr[..n].to_vec())
    }

    /// Write calibration values.
    pub fn write_calibration(&self, device: i32, bank: i32, header: &mut CalHeader, values: &[f32])
        -> Result<()>
    {
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_write_calibration(
                &mut h, device, bank, header, values.as_ptr() as *mut f32, values.len() as c_int,
            )
        })
    }

    /// Whether the last `write_calibration` completed.
    pub fn write_calibration_successful(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_write_calibration_successful(&mut h, device, bank) })
    }

    /// Write calibration points.
    pub fn write_calibration_points(&self, device: i32, bank: i32, header: &mut CalHeader, values: &[f32])
        -> Result<()>
    {
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_write_calibration_points(
                &mut h, device, bank, header, values.as_ptr() as *mut f32, values.len() as c_int,
            )
        })
    }

    /// Whether the last `write_calibration_points` completed.
    pub fn write_calibration_points_successful(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_write_calibration_points_successful(&mut h, device, bank) })
    }

    /// Store calibration to flash.
    pub fn store_calibration(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_store_calibration(&mut h, device, bank) })
    }

    /// Whether calibration is stored.
    pub fn is_calibration_stored(&self, device: i32, bank: i32) -> Result<bool> {
        let mut out: c_int = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_is_calibration_stored(&mut h, device, bank, &mut out) })?;
        Ok(out != 0)
    }

    /// Whether calibration is valid.
    pub fn calibration_is_valid(&self, device: i32, bank: i32) -> Result<bool> {
        let mut out: c_int = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_get_calibration_is_valid(&mut h, device, bank, &mut out) })?;
        Ok(out != 0)
    }

    /// Clear stored calibration.
    pub fn clear_calibration(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_clear_calibration(&mut h, device, bank) })
    }

    /// Request calibration info.
    pub fn request_calibration_info(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_request_calibration_info(&mut h, device, bank) })
    }

    /// Read calibration info header.
    pub fn read_calibration_info(&self, device: i32, bank: i32) -> Result<CalHeader> {
        let mut header: CalHeader = unsafe { std::mem::zeroed() };
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_read_calibration_info(&mut h, device, bank, &mut header) })?;
        Ok(header)
    }
}
```

- [ ] **Step 2: Wire into `src/lib.rs` and add the `raw` re-export module**

Add to `src/lib.rs`:

```rust
mod calibration;

/// Raw `repr(C)` structs used by the settings/calibration/statistics calls.
pub mod raw {
    pub use crate::ffi::{
        neoRADIO2_settings, neoRADIO2_PerfStatistics, neoRADIO2frame_calHeader,
        neoRADIO2AOUT_header,
    };
}
```

- [ ] **Step 3: Verify it compiles**

Run: `cargo build`
Expected: clean.

- [ ] **Step 4: Commit**

```bash
git add src/calibration.rs src/lib.rs
git commit -m "feat(rust): full calibration flow (raw cal header)"
```

---

### Task 10: Adaptive hardware test + example

**Files:**
- Create: `tests/hardware.rs`, `examples/find_and_read.rs`

**Interfaces:**
- Consumes: the full public API.

- [ ] **Step 1: Write `tests/hardware.rs`**

```rust
//! Runs against whatever neoRAD-IO2 device is connected. If nothing is
//! connected, the test prints a note and passes (so it is CI-safe).
use neoradio2::{CalType, Device, DeviceType};

#[test]
fn exercise_connected_device() {
    let infos = Device::find().expect("find");
    let Some(info) = infos.into_iter().next() else {
        eprintln!("no neoRAD-IO2 device connected; skipping hardware test");
        return;
    };
    eprintln!("testing {} {}", info.name(), info.serial());

    let dev = Device::open(&info).expect("open");
    assert!(dev.is_opened().expect("is_opened"));
    dev.chain_identify().expect("chain_identify");
    let count = dev.chain_count(true).expect("chain_count");
    assert!(count >= 1, "expected at least one device in the chain");

    for d in 0..count {
        dev.app_start(d, 0xFF).ok();
        let ty = dev.device_type(d, 0).expect("device_type");
        let sn = dev.serial_number(d, 0).expect("serial_number");
        let (fw_major, fw_minor) = dev.firmware_version(d, 0).expect("firmware_version");
        eprintln!(
            "  device {d}: type={:?} serial={sn} fw={fw_major}.{fw_minor}",
            DeviceType::from_raw(ty as i32)
        );
        // A sensor read is meaningful only for analog devices; ignore errors.
        if dev.request_sensor_data(d, 0x01, CalType::Enabled).is_ok() {
            if let Ok(v) = dev.read_sensor_float(d, 0) {
                eprintln!("    bank0 = {v}");
            }
        }
    }
    // Device closes on drop.
}
```

- [ ] **Step 2: Write `examples/find_and_read.rs`**

```rust
use neoradio2::{CalType, Device};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    for info in Device::find()? {
        println!("{info:?}");
        let dev = Device::open(&info)?;
        dev.chain_identify()?;
        dev.app_start(0, 0xFF)?;
        dev.request_sensor_data(0, 0xFF, CalType::Enabled)?;
        for bank in 0..8 {
            if let Ok(v) = dev.read_sensor_float(0, bank) {
                println!("  bank {bank}: {v}");
            }
        }
    }
    Ok(())
}
```

- [ ] **Step 3: Run the hardware test (skips cleanly with no device)**

Run: `cargo test --test hardware -- --nocapture`
Expected: prints "no ... device connected; skipping" and PASSES when nothing is connected; exercises the device when one is present.

- [ ] **Step 4: Build the example**

Run: `cargo build --example find_and_read`
Expected: clean.

- [ ] **Step 5: Commit**

```bash
git add tests/hardware.rs examples/find_and_read.rs
git commit -m "feat(rust): adaptive hardware test and example"
```

---

### Task 11: Crate docs, clippy/fmt clean, CI workflow

**Files:**
- Modify: `src/lib.rs`
- Create: `.github/workflows/rust.yml`

**Interfaces:** none new.

- [ ] **Step 1: Add crate-level docs + deny-missing-docs to `src/lib.rs`**

Set the top of `src/lib.rs` to:

```rust
//! Safe Rust bindings for the Intrepid Control Systems neoRAD-IO2 device family.
//!
//! ```no_run
//! use neoradio2::{Device, CalType};
//! for info in Device::find()? {
//!     let dev = Device::open(&info)?;
//!     dev.chain_identify()?;
//!     dev.app_start(0, 0xFF)?;
//!     dev.request_sensor_data(0, 0xFF, CalType::Enabled)?;
//!     println!("{}", dev.read_sensor_float(0, 0)?);
//! }
//! # Ok::<(), neoradio2::Error>(())
//! ```
#![warn(missing_docs)]
```

(Keep the existing `pub mod ffi;`, `mod ...;`, and `pub use ...;` lines below it. `ffi` is raw; add `#[doc(hidden)]` above `pub mod ffi;`.)

- [ ] **Step 2: Run fmt and clippy**

Run: `cargo fmt` then `cargo clippy --all-targets -- -D warnings`
Expected: no warnings. Fix any clippy findings (e.g. needless casts) until clean.

- [ ] **Step 3: Write `.github/workflows/rust.yml`**

```yaml
name: Rust

on:
  push:
    paths: ['src/**', 'build.rs', 'Cargo.toml', 'tests/**', 'examples/**', '.github/workflows/rust.yml']
  pull_request:
    paths: ['src/**', 'build.rs', 'Cargo.toml', 'tests/**', 'examples/**', '.github/workflows/rust.yml']

jobs:
  build:
    strategy:
      fail-fast: false
      matrix:
        os: [ubuntu-latest, windows-latest, macOS-latest]
    runs-on: ${{ matrix.os }}
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive
      - name: Install Linux build deps
        if: runner.os == 'Linux'
        run: sudo apt-get update && sudo apt-get install -y libudev-dev libusb-1.0-0-dev
      - uses: dtolnay/rust-toolchain@stable
        with:
          components: rustfmt, clippy
      - run: cargo fmt --check
      - run: cargo clippy --all-targets -- -D warnings
      - run: cargo build --verbose
      - run: cargo test --verbose

  bindgen-sync:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive
      - run: sudo apt-get update && sudo apt-get install -y libudev-dev libusb-1.0-0-dev llvm-dev libclang-dev clang
      - uses: dtolnay/rust-toolchain@stable
      - name: Regenerate bindings and check for drift
        run: |
          cargo build --features bindgen
          git diff --exit-code src/ffi.rs
```

- [ ] **Step 4: Validate the workflow YAML**

Run: `python -c "import yaml; yaml.safe_load(open('.github/workflows/rust.yml')); print('ok')"`
Expected: `ok`.

- [ ] **Step 5: Commit**

```bash
git add src/lib.rs .github/workflows/rust.yml
git commit -m "feat(rust): crate docs, clippy-clean, CI workflow"
```

---

## Self-Review

**Spec coverage:**
- §1 layout → Tasks 1–11 create exactly the files listed. ✓
- §2 build/linking → Task 1 build.rs (cmake + per-OS C++ runtime + system libs). ✓
- §3 committed FFI + `bindgen` feature + CI drift check → Task 1 (generate/commit) + Task 11 (`bindgen-sync` job). ✓
- §4 Error/Result → Task 2; DeviceInfo/Device/Drop/Send-not-Sync → Task 4; enums + global blocking → Task 3; raw structs under `raw` → Task 9. ✓
- §4 coverage map (all ~50 fns) → find/open/close/state (4), chain/app/boot (5), identity (6), sensor (7), LED/settings/stats/status (8), calibration (9). Every C function in the map has a method. ✓
- §5 testing (no-hardware always-run + adaptive hardware skip) → Task 4 (`tests/api.rs`), Task 10 (`tests/hardware.rs`). ✓
- §6 CI → Task 11. ✓

**Placeholder scan:** No TBD/TODO; every code step shows complete code. One explicit *verification* note (Task 8 Step 3) about matching `ffi::CommandStatus`/`StatusType`'s generated Rust type — this is a real inspection step, not a placeholder, because bindgen may emit those either as `c_int` type aliases or as newtype enums depending on version; the implementer reads `src/ffi.rs` and matches. ✓

**Type consistency:** `Device.handle: ffi::neoradio2_handle` used consistently; `check() -> Result<()>`; `DeviceInfo(pub(crate) ffi::Neoradio2DeviceInfo)` used by `open`; enums expose `as_raw()/from_raw()` used uniformly; `CalHeader` alias in Task 9 matches `ffi::neoRADIO2frame_calHeader`. ✓

**Known risk carried into execution:** exact static-lib file names/paths under the CMake build dir are discovered by the globbing loop in `build.rs` (Task 1 Step 3); if the glob is too broad/narrow, adjust the `is_static` predicate after inspecting the first build's `target/.../build` tree.
