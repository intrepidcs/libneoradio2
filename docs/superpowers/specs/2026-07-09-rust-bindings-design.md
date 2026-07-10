# Rust bindings for libneoradio2 — design

## Overview

Add safe, idiomatic Rust bindings for the libneoradio2 C API as a single crate
named `neoradio2`, with `Cargo.toml` at the repository root. The crate builds
the existing C/C++ library from source (via CMake) and exposes a safe `Device`
API over an internal FFI layer.

### Goals

- One crate, safe idiomatic API (`Device`, `Result<T, Error>`, RAII close on `Drop`, Rust enums).
- 100% coverage of the C API surface (all ~50 `neoradio2_*` functions callable safely).
- `cargo build` works on Linux/macOS/Windows with only CMake + a C/C++ toolchain — **no libclang** in the default path.
- Coexist with the existing root CMake and Python/scikit-build builds without disturbing them.

### Non-goals (future work)

- Fully idiomatic Rust models of the settings/calibration/statistics structs. v1 exposes those as `repr(C)` types re-exported under `neoradio2::raw` (see §4).
- Async API. The C API is blocking/polling; v1 mirrors that.
- Publishing to crates.io (can follow once the API settles).

## §1 — Crate layout

Single crate rooted at the repo root (per request):

```
Cargo.toml            # package "neoradio2"
build.rs              # cmake build + linking; optional bindgen regen
src/
  lib.rs              # crate root: module wiring, re-exports, crate docs
  ffi.rs              # committed bindgen output (raw extern "C" + repr(C) types)
  error.rs            # Error enum + Result<T>; maps C return codes
  types.rs            # DeviceType, CalType, LedMode, StatusType, CommandStatus enums
  device.rs           # Device (owns handle) + DeviceInfo + core methods
  settings.rs         # settings read/write wrappers (raw structs)
  calibration.rs      # calibration flow wrappers (raw structs)
  statistics.rs       # statistics wrapper (raw struct)
examples/
  find_and_read.rs
tests/
  api.rs              # no-hardware unit tests
  hardware.rs         # adaptive tests: run against whatever is connected, skip if none
```

Cargo ignores `CMakeLists.txt`/`pyproject.toml`, so the C and Python builds are
untouched. Repo additions: the Rust files above and `/target` added to
`.gitignore`.

## §2 — Build & linking (`build.rs`)

- Use the `cmake` crate to run `libneoradio2/CMakeLists.txt`, which already
  builds `hidapi` (per-OS sources + udev/libusb) and the static `libneoradio2`.
  Build `libneoradio2` with `-DBUILD_SHARED_LIBS=OFF` (default) so we link static archives.
- Emit link directives for the two static libs plus, because the library is
  **C++**, the C++ runtime and hidapi's platform libraries:
  - **Linux:** `static=libneoradio2`, `static=hidapi`, `dylib=udev`, `dylib=stdc++`
  - **macOS:** `static=…`, frameworks `IOKit` / `CoreFoundation` / `AppKit`, `dylib=c++`
  - **Windows (MSVC):** `static=…`, `setupapi`; C++ runtime auto-linked
- The exact static-lib names/paths come from the `cmake` crate's output dir
  (`{out}/build/...`); resolve them from there rather than hardcoding.
- Build-time requirements: CMake + a C/C++ toolchain (same as the Python wheels).
  Submodules (`hidapi`, `neoRAD-IO2-FrameDescription`) must be checked out.

## §3 — FFI layer (`src/ffi.rs`, committed)

- Generated once with bindgen and committed. bindgen config:
  - `header` = `libneoradio2/include/libneoradio2.h` with include paths for the
    two submodule header dirs.
  - Allowlist `neoradio2_.*` functions, `NEORADIO2_.*` constants, and the types
    they reference (`Neoradio2DeviceInfo`, the enums, and the frame-description
    structs `neoRADIO2_settings`, `neoRADIO2frame_calHeader`,
    `neoRADIO2_PerfStatistics`, `neoRADIO2AOUT_header`, etc.).
  - The union/bitfield structs come out correct because bindgen handles them.
- Module header: `#![allow(non_camel_case_types, non_snake_case, non_upper_case_globals)]`.
- A `bindgen` cargo feature makes `build.rs` regenerate and overwrite
  `src/ffi.rs`. A CI job runs `cargo build --features bindgen` then
  `git diff --exit-code` to catch drift. Default users never need libclang.

## §4 — Safe API

### Error handling (`error.rs`)

```rust
#[non_exhaustive]
pub enum Error {
    Failure,        // NEORADIO2_FAILURE (1) and ERR_FAILURE (4)
    WouldBlock,     // NEORADIO2_ERR_WBLOCK (2)
    InProgress,     // NEORADIO2_ERR_INPROGRESS (3)
    NulByte,        // interior NUL when building a C string, etc.
}
impl std::error::Error for Error {}
pub type Result<T> = std::result::Result<T, Error>;
```

An internal helper maps a C `int` return to `Result<()>` (0 → `Ok`).

### DeviceInfo & Device (`device.rs`)

- `DeviceInfo { name: String, serial: String, vendor_id: i32, product_id: i32 }`,
  built from the C `Neoradio2DeviceInfo` (fixed `char[64]` arrays → `String`). It
  also keeps the raw struct internally so `open` can pass it back to C.
- `Device` owns the `neoradio2_handle`; `Drop` calls `neoradio2_close`.
  - `Device::find() -> Result<Vec<DeviceInfo>>` (uses a fixed-size buffer of
    `NEORADIO2_MAX_DEVS`, clamps to the returned count).
  - `Device::open(&DeviceInfo) -> Result<Device>`.
  - Methods mirror the C API and return `Result<T>` (see §4 coverage list).
- `Device` is `Send` but **not `Sync`**: the C library is not safe for concurrent
  same-handle calls, and blocking mode is global. Documented on the type.

### Global blocking mode

`neoradio2_set_blocking`/`neoradio2_is_blocking` are process-global, so they are
free functions, not `Device` methods:

```rust
pub fn set_blocking(blocking: bool, timeout: Duration);
pub fn is_blocking() -> bool;
```

### Enums (`types.rs`)

`DeviceType` (TC/DIO/PWRRLY/AIN/AOUT/CANHUB/BADGE/HOST), `CalType`
(Enabled/NoCal/NoCalEnhanced), `LedMode` (Off/On/BlinkOnce/BlinkDurationMs),
`StatusType`, `CommandStatus` — each with `From`/`TryFrom` conversions to the C
`int`/enum values.

### Struct-heavy calls: raw structs now (decided)

Settings, calibration, and statistics functions are fully wrapped and return
`Result`, but their struct parameters/returns use the bindgen `repr(C)` types
re-exported under `neoradio2::raw`:

```rust
pub mod raw { pub use crate::ffi::{neoRADIO2_settings, neoRADIO2frame_calHeader,
                                   neoRADIO2_PerfStatistics, /* … */}; }

impl Device {
    pub fn read_settings(&self, device: i32, bank: i32) -> Result<raw::neoRADIO2_settings>;
    pub fn write_settings(&self, device: i32, bank: i32, s: &raw::neoRADIO2_settings) -> Result<()>;
    pub fn read_statistics(&self, device: i32, bank: i32) -> Result<raw::neoRADIO2_PerfStatistics>;
    // calibration: request/read/write/store/clear/info, all taking &mut raw::…calHeader
}
```

A few cheap ergonomic helpers may be added (e.g. read a channel name as a
`String`), but idiomatic modeling of these structs is deferred.

## §4 coverage — C function → safe method map

- Discovery/lifecycle: `find`, `open`, `close` (Drop), `is_opened`, `is_closed`.
- Chain: `chain_identify`, `chain_is_identified`, `get_chain_count`.
- App/boot: `app_start`, `app_is_started`, `enter_bootloader`.
- Identity: `get_serial_number`, `get_manufacturer_date`, `get_firmware_version`,
  `get_hardware_revision`, `get_device_type`, `request_pcbsn`, `get_pcbsn`.
- Sensor: `request_sensor_data`, `read_sensor_float`, `read_sensor_array`,
  `write_sensor`, `write_sensor_successful`.
- Settings: `request_settings`, `read_settings`, `write_settings`,
  `write_settings_successful`, `write_default_settings`.
- Calibration: `request_calibration`, `read_calibration_array`,
  `request_calibration_points`, `read_calibration_points_array`,
  `write_calibration`, `write_calibration_points`, `*_successful`,
  `store_calibration`, `is_calibration_stored`, `get_calibration_is_valid`,
  `clear_calibration`, `request_calibration_info`, `read_calibration_info`.
- LED: `toggle_led`, `toggle_led_successful`.
- Status/stats: `get_status`, `request_statistics`, `read_statistics`.
- Global: `set_blocking`, `is_blocking`.

## §5 — Testing

- **No-hardware unit tests (`tests/api.rs`)** — always run:
  - C-return-code → `Error` mapping.
  - `DeviceInfo` parsing from a fabricated `Neoradio2DeviceInfo`.
  - Enum `From`/`TryFrom` round-trips.
  - `Device::find()` returns an empty `Vec` (not an error) with nothing plugged in.
- **Adaptive hardware tests (`tests/hardware.rs`)** — run against whatever is
  connected, **skip cleanly when nothing is** (Rust has no built-in skip, so the
  test early-returns with an `eprintln!("no device connected; skipping")` and
  passes). When device(s) are present it: `find` → `open` → `chain_identify` →
  `get_chain_count` → `app_start` → read identity (serial/fw/hw/type) → attempt a
  sensor read (per-device-type failures handled gracefully) → `close`. This means
  `cargo test` passes in CI (no hardware) and exercises real devices locally,
  mirroring how the Python hardware checks were run this session.
- Doctests on the public API; `examples/find_and_read.rs`.

## §6 — CI

A Rust workflow (`.github/workflows/rust.yml`) on Linux/Windows/macOS:

- checkout with `submodules: recursive`; ensure CMake present (runners have it);
  on Linux `apt-get install libudev-dev libusb-1.0-0-dev` (matches the docs job).
- `cargo build`, `cargo test` (no-hardware tests pass; hardware tests self-skip),
  `cargo fmt --check`, `cargo clippy -- -D warnings`.
- A `bindgen`-sync job: `cargo build --features bindgen` then `git diff --exit-code src/ffi.rs`.

## §7 — Publishing to crates.io

The crate must be publishable to crates.io, which packages the crate directory
into a self-contained tarball that any user builds without our git history or
submodules. This drives several requirements:

- **Bundle the C sources.** `build.rs` compiles the vendored C/C++ library, so
  the tarball must contain `libneoradio2/{src,include,CMakeLists.txt}` plus the
  two submodules' working-tree files (`neoRAD-IO2-FrameDescription/`, `hidapi/`).
  Use an explicit `include = [...]` list in `Cargo.toml` — explicit includes pull
  in submodule files regardless of git-submodule/`.gitignore` status, and exclude
  the unrelated `python/`, `doc/`, `example/`, `test/` (incl. the `test/ice`
  submodule) trees. Bundled payload is ~2.7 MB, well under the crates.io 10 MB
  limit. hidapi is vendored wholesale (its own CMakeLists references many of its
  subdirs) minus nothing needed — robustness over minimalism.
- **Committed FFI is the crates.io-safe path.** `src/ffi.rs` is committed, so a
  default `cargo build`/`cargo install neoradio2` never needs libclang. `bindgen`
  stays an **optional** build-dependency behind the `bindgen` feature; it is never
  compiled on the default path, so it does not gate publication or downstream
  builds.
- **docs.rs.** docs.rs builds documentation in a sandbox without libudev/network
  and only needs `cargo doc` (which compiles but does not link the native lib).
  `build.rs` returns early when the `DOCS_RS` env var is set, skipping the CMake
  build and all link directives so doc builds succeed. `extern` declarations need
  no symbols at doc-compile time.
- **Manifest metadata** (required/expected for a good crates.io page): `name`,
  `version`, `edition`, `license = "MIT"`, `description`, `repository`,
  `documentation = "https://docs.rs/neoradio2"`, `homepage`, `readme = "README.md"`,
  `keywords`, `categories`, `rust-version`. A Rust "## Rust" section is added to
  the root `README.md` so the crates.io landing page has Rust-focused install/use
  content (the existing README is C/Python-oriented).
- **Versioning.** The Rust API is new and unproven, so the crate starts at
  **0.1.0** (SemVer: pre-1.0 signals the API may change) rather than tracking the
  C library's 1.4.x. This is independent of the C/Python package version.
- **Bundled-license note.** Our wrapper is MIT. Vendored hidapi is tri-licensed
  (BSD / GPLv3 / original HIDAPI) — the permissive BSD option applies; its license
  files ship in the tarball. The frame-description headers are Intrepid's.
- **Acceptance gate.** `cargo publish --dry-run` (which packages *and* builds the
  extracted tarball) must succeed on Linux/Windows/macOS, and `cargo package
  --list` must show the C sources + both submodules. The `include` list is tuned
  against that output until the packaged crate builds cleanly.

## Open questions / risks

- Static-linking a C++ library into a Rust binary requires the correct C++ runtime
  link flag per platform (`stdc++` vs `c++`); build.rs must branch on target.
- The committed `src/ffi.rs` can drift if the C headers change; mitigated by the
  CI bindgen-sync check.
- `NEORADIO2_MAX_DEVS` (8) bounds the `find` buffer; more than 8 USB devices would
  be truncated (matches the C/Python behavior after the review fixes).
- The `include` list can omit a C source hidapi's CMake needs, breaking the
  packaged build even when the in-repo build works; the `cargo publish --dry-run`
  acceptance gate (§7) catches this because it builds the extracted tarball.
- docs.rs would fail if `build.rs` tried the native build there; mitigated by the
  `DOCS_RS` early return (§7).
