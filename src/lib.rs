//! Rust bindings for the Intrepid Control Systems neoRAD-IO2 device library.

pub mod ffi;
mod device;
mod error;
mod settings;
mod types;

pub use device::{Device, DeviceInfo};
pub use error::{Error, Result};
pub(crate) use error::check;
pub use types::{CalType, CommandStatus, DeviceType, LedMode, StatusType, is_blocking, set_blocking};
