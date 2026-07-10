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
