//! Rust bindings for the Intrepid Control Systems neoRAD-IO2 device library.

pub mod ffi;
mod calibration;
mod device;
mod error;
mod settings;
mod types;

pub use device::{Device, DeviceInfo};
pub use error::{Error, Result};
pub(crate) use error::check;
pub use types::{CalType, CommandStatus, DeviceType, LedMode, StatusType, is_blocking, set_blocking};

/// Raw `repr(C)` structs used by the settings/calibration/statistics calls.
pub mod raw {
    pub use crate::ffi::{
        neoRADIO2_settings, neoRADIO2_PerfStatistics, neoRADIO2frame_calHeader,
        neoRADIO2AOUT_header,
    };
}
