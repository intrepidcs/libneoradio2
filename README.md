# libneoradio2

![Build](https://github.com/intrepidcs/libneoradio2/actions/workflows/wheels.yml/badge.svg)

C library (with Python bindings) for communicating with the Intrepid Control
Systems [neoRAD-IO2](https://www.intrepidcs.com/products/analog-daq-devices/rad-io2-series/)
product line — a family of ruggedized, isolated analog / digital / temperature
DAQ devices that connect over USB. Up to eight devices can be daisy-chained,
with 2.5 kV bank-to-bank isolation.

## Install (Python)

```
pip install neoradio2
```

```python
import neoradio2

for device in neoradio2.find():
    print(device)
    handle = neoradio2.open(device)
    # ... talk to the device ...
    neoradio2.close(handle)
```

More detail: [`python/README.md`](python/README.md) ·
[Python docs](https://intrepidcs.github.io/libneoradio2/python/docs/html/) ·
[C library docs](https://intrepidcs.github.io/libneoradio2/doc/html/libneoradio2_8h.html)

## Rust

Safe Rust bindings are published as the [`neoradio2`](https://crates.io/crates/neoradio2) crate.

```toml
[dependencies]
neoradio2 = "0.1"
```

```rust
use neoradio2::{Device, CalType};

for info in Device::find()? {
    let dev = Device::open(&info)?;
    dev.chain_identify()?;
    dev.app_start(0, 0xFF)?;
    dev.request_sensor_data(0, 0xFF, CalType::Enabled)?;
    println!("{} = {}", info.serial(), dev.read_sensor_float(0, 0)?);
}
# Ok::<(), neoradio2::Error>(())
```

Building the crate compiles the bundled C library from source, so it needs CMake
and a C/C++ toolchain (the same as a source build above); no libclang is
required. On Linux install `libudev-dev`.

## Build from source

This repository uses git submodules for the HID backend and the frame
description headers, so clone recursively (or initialize them after cloning):

```
git clone --recursive https://github.com/intrepidcs/libneoradio2.git
# or, in an existing checkout:
git submodule update --init --recursive
```

### C library

```
cmake -S . -B build
cmake --build build
```

Add `-DBUILD_SHARED_LIBS=ON` for a shared library, or `-DBUILD_PYTHON_BINDINGS=ON`
to build the Python extension via CMake directly.

**Linux:** install the udev rules so devices are accessible as a normal user —
copy [`99-intrepidcs.rules`](99-intrepidcs.rules) to `/etc/udev/rules.d/`, run
`sudo udevadm control --reload-rules && sudo udevadm trigger`, and add your user
to the `users` group (`sudo usermod -aG users $USER`).
Build dependencies: `sudo apt install cmake build-essential libudev-dev`
(Debian/Ubuntu) or `sudo dnf install cmake gcc-c++ libudev-devel` (Fedora).

**Windows:** Visual Studio 2019 or newer (MSVC) with CMake.

## Repository layout

| Path | Contents |
|------|----------|
| `libneoradio2/` | C/C++ library sources, headers, and C examples |
| `python/` | pybind11 bindings, Python examples, tests, and docs |
| `libneoradio2/hidapi/` | HID backend (submodule) |
| `libneoradio2/neoRAD-IO2-FrameDescription/` | Wire-frame struct definitions (submodule) |

## License

MIT — see [LICENSE](LICENSE). Copyright © Intrepid Control Systems, Inc.
