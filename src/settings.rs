use crate::{check, ffi, CommandStatus, Device, LedMode, Result, StatusType};
use std::os::raw::c_int;

impl Device {
    /// Toggle LEDs on `device`/`bank`.
    pub fn toggle_led(
        &self,
        device: i32,
        bank: i32,
        mode: LedMode,
        led_enables: i32,
        ms: i32,
    ) -> Result<()> {
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
    pub fn write_settings(
        &self,
        device: i32,
        bank: i32,
        settings: &ffi::neoRADIO2_settings,
    ) -> Result<()> {
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
    pub fn read_statistics(&self, device: i32, bank: i32) -> Result<ffi::neoRADIO2_PerfStatistics> {
        let mut s: ffi::neoRADIO2_PerfStatistics = unsafe { std::mem::zeroed() };
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_read_statistics(&mut h, device, bank, &mut s) })?;
        Ok(s)
    }

    /// Query a command status.
    pub fn status(
        &self,
        device: i32,
        bank: i32,
        bitfield: bool,
        ty: StatusType,
    ) -> Result<CommandStatus> {
        let mut out: ffi::CommandStatus = 0;
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_get_status(
                &mut h,
                device,
                bank,
                bitfield as c_int,
                ty.as_raw() as ffi::StatusType,
                &mut out,
            )
        })?;
        Ok(CommandStatus::from_raw(out as i32).unwrap_or(CommandStatus::Error))
    }
}
