use crate::ffi;
use std::time::Duration;

macro_rules! int_enum {
    ($(#[$m:meta])* $name:ident { $($(#[$vm:meta])* $variant:ident = $val:expr),* $(,)? }) => {
        $(#[$m])*
        #[derive(Debug, Clone, Copy, PartialEq, Eq)]
        pub enum $name { $($(#[$vm])* $variant = $val),* }
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
    DeviceType {
        /// Thermocouple module.
        Tc = 0,
        /// Digital I/O module.
        Dio = 1,
        /// Power relay module.
        PwrRly = 2,
        /// Analog input module.
        Ain = 3,
        /// Analog output module.
        Aout = 4,
        /// CAN hub module.
        CanHub = 5,
        /// Badge module.
        Badge = 6,
        /// Host (non-module) device.
        Host = 0xFF
    }
);
int_enum!(
    /// Sensor read calibration mode.
    CalType {
        /// Apply stored calibration.
        Enabled = 0,
        /// Return raw, uncalibrated data.
        NoCal = 1,
        /// Return raw data with enhanced (extended-range) scaling.
        NoCalEnhanced = 2
    }
);
int_enum!(
    /// LED toggle mode.
    LedMode {
        /// Turn the LED(s) off.
        Off = 0,
        /// Turn the LED(s) on.
        On = 1,
        /// Blink the LED(s) once.
        BlinkOnce = 2,
        /// Blink the LED(s) for a given duration in milliseconds.
        BlinkDurationMs = 3
    }
);
int_enum!(
    /// Status query type (see `Device::status`).
    StatusType {
        /// Chain identification status.
        Chain = 0,
        /// Application firmware start status.
        AppStart = 1,
        /// PCB serial number read status.
        Pcbsn = 2,
        /// Sensor read status.
        SensorRead = 3,
        /// Sensor write status.
        SensorWrite = 4,
        /// Settings read status.
        SettingsRead = 5,
        /// Settings write status.
        SettingsWrite = 6,
        /// Calibration write status.
        Calibration = 7,
        /// Calibration points write status.
        CalibrationPoints = 8,
        /// Calibration stored status.
        CalibrationStored = 9,
        /// Calibration info status.
        CalibrationInfo = 10,
        /// LED toggle status.
        LedToggle = 11,
    }
);
int_enum!(
    /// Command status returned by `Device::status`.
    CommandStatus {
        /// The command has not yet finished.
        InProgress = 0,
        /// The command completed successfully.
        Finished = 1,
        /// The command failed.
        Error = 2
    }
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
        for (v, t) in [
            (0, DeviceType::Tc),
            (3, DeviceType::Ain),
            (6, DeviceType::Badge),
        ] {
            assert_eq!(DeviceType::from_raw(v), Some(t));
            assert_eq!(t.as_raw(), v);
        }
        assert_eq!(DeviceType::from_raw(42), None);
        // Host uses a non-sequential C value (0xFF); verify accessors and the
        // native discriminant cast all agree.
        assert_eq!(DeviceType::from_raw(0xFF), Some(DeviceType::Host));
        assert_eq!(DeviceType::Host.as_raw(), 0xFF);
        assert_eq!(DeviceType::Host as i32, 0xFF);
    }
    #[test]
    fn blocking_round_trips() {
        set_blocking(true, std::time::Duration::from_millis(500));
        assert!(is_blocking());
        set_blocking(false, std::time::Duration::from_millis(500));
        assert!(!is_blocking());
    }
    #[test]
    fn caltype_values() {
        assert_eq!(CalType::Enabled.as_raw(), 0);
        assert_eq!(CalType::NoCal.as_raw(), 1);
    }
}
