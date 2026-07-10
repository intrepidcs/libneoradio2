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

mod calibration;
mod device;
mod error;
// `ffi` is bindgen-generated (`build.rs` / `cargo build --features bindgen`); it's
// not hand-maintained, so lints that would otherwise call for real code fixes
// (e.g. `useless_transmute` on bindgen's bitfield-accessor boilerplate) are
// silenced here rather than edited into generated output.
#[doc(hidden)]
#[allow(clippy::useless_transmute)]
#[rustfmt::skip]
pub mod ffi;
mod settings;
mod types;

pub use device::{Device, DeviceInfo};
pub(crate) use error::check;
pub use error::{Error, Result};
pub use types::{
    is_blocking, set_blocking, CalType, CommandStatus, DeviceType, LedMode, StatusType,
};

/// Raw `repr(C)` structs used by the settings/calibration/statistics calls.
pub mod raw {
    pub use crate::ffi::{
        neoRADIO2AOUT_header, neoRADIO2_PerfStatistics, neoRADIO2_settings,
        neoRADIO2frame_calHeader,
    };
}
