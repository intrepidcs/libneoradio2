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

## Linux: device access (udev rules)

> **On Linux you must install the udev rules once** so the devices are
> accessible without root. This applies to **all** bindings — Python, Rust, and
> C alike. (No special setup is needed on Windows or macOS.)

[`99-intrepidcs.rules`](99-intrepidcs.rules) lives in the repository root:

```
sudo cp 99-intrepidcs.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger
sudo usermod -aG users $USER    # then log out and back in
```

Re-plug the device after installing the rules. See
[`python/docs`](https://intrepidcs.github.io/libneoradio2/python/docs/html/installation.html)
for the hand-written rule if you'd rather not download the file.

## Rust

Safe Rust bindings live in the `neoradio2` crate. It is **not yet published to
crates.io**, so add it as a git dependency:

```toml
[dependencies]
neoradio2 = { git = "https://github.com/intrepidcs/libneoradio2" }
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

**Rust API docs:** <https://intrepidcs.github.io/libneoradio2/rust/doc/neoradio2/>

Building the crate compiles the bundled C library from source, so it needs CMake
and a C/C++ toolchain (the same as a source build below); no libclang is
required. On Linux also install `libudev-dev` and the
[udev rules](#linux-device-access-udev-rules) above.

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

**Linux:** build dependencies are `sudo apt install cmake build-essential libudev-dev`
(Debian/Ubuntu) or `sudo dnf install cmake gcc-c++ libudev-devel` (Fedora). To
access devices as a normal user, install the
[udev rules](#linux-device-access-udev-rules) described above.

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
