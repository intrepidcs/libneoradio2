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
