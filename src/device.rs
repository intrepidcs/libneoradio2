use crate::{check, ffi, Result};
use std::os::raw::{c_char, c_int, c_uint};

/// The raw C handle type. `neoradio2_handle` is `#define neoradio2_handle long`
/// in the C headers, so bindgen inlines it as `c_long` rather than emitting an
/// alias; we name it here for readability within this crate.
pub(crate) type Handle = std::os::raw::c_long;

const MAX_DEVS: usize = ffi::NEORADIO2_MAX_DEVS as usize;

fn cstr_to_string(buf: &[c_char]) -> String {
    let bytes: Vec<u8> = buf
        .iter()
        .take_while(|&&c| c != 0)
        .map(|&c| c as u8)
        .collect();
    String::from_utf8_lossy(&bytes).into_owned()
}

/// Information about a discovered neoRAD-IO2 USB device.
#[derive(Clone)]
pub struct DeviceInfo(pub(crate) ffi::Neoradio2DeviceInfo);

impl DeviceInfo {
    /// Product name (e.g. "neoRAD-IO2-AIN").
    pub fn name(&self) -> String {
        cstr_to_string(&self.0.name)
    }
    /// Serial string (e.g. "IB0370").
    pub fn serial(&self) -> String {
        cstr_to_string(&self.0.serial_str)
    }
    /// USB vendor id.
    pub fn vendor_id(&self) -> i32 {
        self.0.vendor_id
    }
    /// USB product id.
    pub fn product_id(&self) -> i32 {
        self.0.product_id
    }
}

impl std::fmt::Debug for DeviceInfo {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("DeviceInfo")
            .field("name", &self.name())
            .field("serial", &self.serial())
            .field("vendor_id", &format_args!("{:#06x}", self.vendor_id()))
            .field("product_id", &format_args!("{:#06x}", self.product_id()))
            .finish()
    }
}

/// An open connection to a neoRAD-IO2 chain (closed automatically on drop).
///
/// `Device` is `Send` but not `Sync`: the underlying C library is not safe for
/// concurrent calls on the same handle, and blocking mode is process-global.
pub struct Device {
    pub(crate) handle: Handle,
    // `Handle` (`c_long`) is itself `Sync`, so without this marker the compiler
    // would auto-derive `Sync` for `Device` too. `*const ()` is neither `Send`
    // nor `Sync`, which suppresses both auto-impls; `Send` is then restored
    // explicitly below, leaving `Device` correctly `Send` but not `Sync`.
    _not_sync: std::marker::PhantomData<*const ()>,
}

// Safe to move a handle between threads; not safe to share (&Device) across them.
unsafe impl Send for Device {}

impl Device {
    /// Enumerate attached neoRAD-IO2 USB devices.
    pub fn find() -> Result<Vec<DeviceInfo>> {
        let mut buf: [ffi::Neoradio2DeviceInfo; MAX_DEVS] = unsafe { std::mem::zeroed() };
        let mut count: c_uint = MAX_DEVS as c_uint;
        check(unsafe { ffi::neoradio2_find(buf.as_mut_ptr(), &mut count) })?;
        let n = (count as usize).min(MAX_DEVS);
        Ok(buf[..n].iter().map(|raw| DeviceInfo(*raw)).collect())
    }

    /// Open a device discovered by [`find`](Device::find).
    pub fn open(info: &DeviceInfo) -> Result<Device> {
        let mut raw = info.0;
        let mut handle: Handle = -1;
        check(unsafe { ffi::neoradio2_open(&mut handle, &mut raw) })?;
        Ok(Device {
            handle,
            _not_sync: std::marker::PhantomData,
        })
    }

    /// Whether the device is currently open.
    pub fn is_opened(&self) -> Result<bool> {
        let mut out: c_int = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_is_opened(&mut h, &mut out) })?;
        Ok(out != 0)
    }

    /// Whether the device is currently closed.
    pub fn is_closed(&self) -> Result<bool> {
        let mut out: c_int = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_is_closed(&mut h, &mut out) })?;
        Ok(out != 0)
    }
}

impl Device {
    /// Identify the device chain.
    pub fn chain_identify(&self) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_chain_identify(&mut h) })
    }

    /// Whether the chain has been identified.
    pub fn chain_is_identified(&self) -> Result<bool> {
        let mut out: c_int = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_chain_is_identified(&mut h, &mut out) })?;
        Ok(out != 0)
    }

    /// Number of devices in the chain. `identify` re-identifies first.
    pub fn chain_count(&self, identify: bool) -> Result<i32> {
        let mut out: c_int = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_get_chain_count(&mut h, &mut out, identify as c_int) })?;
        Ok(out)
    }

    /// Start application firmware on `device`/`bank` (bank is a bitmask).
    pub fn app_start(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_app_start(&mut h, device, bank) })
    }

    /// Whether application firmware is running on `device`/`bank`.
    pub fn app_is_started(&self, device: i32, bank: i32) -> Result<bool> {
        let mut out: c_int = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_app_is_started(&mut h, device, bank, &mut out) })?;
        Ok(out != 0)
    }

    /// Enter the bootloader on `device`/`bank`.
    pub fn enter_bootloader(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_enter_bootloader(&mut h, device, bank) })
    }
}

impl Device {
    /// Serial number (base-10) of `device`/`bank`.
    pub fn serial_number(&self, device: i32, bank: i32) -> Result<u32> {
        let mut out: c_uint = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_get_serial_number(&mut h, device, bank, &mut out) })?;
        Ok(out)
    }

    /// Manufacturing date `(year, month, day)`.
    pub fn manufacturer_date(&self, device: i32, bank: i32) -> Result<(i32, i32, i32)> {
        let (mut y, mut m, mut d) = (0, 0, 0);
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_get_manufacturer_date(&mut h, device, bank, &mut y, &mut m, &mut d)
        })?;
        Ok((y, m, d))
    }

    /// Firmware version `(major, minor)`.
    pub fn firmware_version(&self, device: i32, bank: i32) -> Result<(i32, i32)> {
        let (mut major, mut minor) = (0, 0);
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_get_firmware_version(&mut h, device, bank, &mut major, &mut minor)
        })?;
        Ok((major, minor))
    }

    /// Hardware revision `(major, minor)`.
    pub fn hardware_revision(&self, device: i32, bank: i32) -> Result<(i32, i32)> {
        let (mut major, mut minor) = (0, 0);
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_get_hardware_revision(&mut h, device, bank, &mut major, &mut minor)
        })?;
        Ok((major, minor))
    }

    /// Device type code (see [`DeviceType::from_raw`](crate::DeviceType::from_raw)).
    pub fn device_type(&self, device: i32, bank: i32) -> Result<u32> {
        let mut out: c_uint = 0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_get_device_type(&mut h, device, bank, &mut out) })?;
        Ok(out)
    }

    /// Request the PCB serial number (call before [`pcbsn`](Device::pcbsn)).
    pub fn request_pcbsn(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_request_pcbsn(&mut h, device, bank) })
    }

    /// PCB serial number string.
    pub fn pcbsn(&self, device: i32, bank: i32) -> Result<String> {
        let mut buf = [0 as c_char; 17];
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_get_pcbsn(&mut h, device, bank, buf.as_mut_ptr()) })?;
        Ok(cstr_to_string(&buf))
    }
}

use crate::CalType;

impl Device {
    /// Request a fresh sensor sample for `device`/`bank`.
    pub fn request_sensor_data(&self, device: i32, bank: i32, cal: CalType) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_request_sensor_data(&mut h, device, bank, cal.as_raw()) })
    }

    /// Read a single sensor value as `f32`.
    pub fn read_sensor_float(&self, device: i32, bank: i32) -> Result<f32> {
        let mut out: f32 = 0.0;
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_read_sensor_float(&mut h, device, bank, &mut out) })?;
        Ok(out)
    }

    /// Read the raw sensor sample as an array of integers (one per byte).
    pub fn read_sensor_array(&self, device: i32, bank: i32) -> Result<Vec<i32>> {
        let mut arr = [0 as c_int; 64];
        let mut len: c_int = arr.len() as c_int;
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_read_sensor_array(&mut h, device, bank, arr.as_mut_ptr(), &mut len)
        })?;
        let n = (len.max(0) as usize).min(arr.len());
        Ok(arr[..n].to_vec())
    }

    /// Write sensor/actuator data for `device`/`bank`.
    pub fn write_sensor(&self, device: i32, bank: i32, data: &[u8]) -> Result<()> {
        let mut h = self.handle;
        check(unsafe {
            ffi::neoradio2_write_sensor(
                &mut h,
                device,
                bank,
                data.as_ptr() as *mut u8,
                data.len() as c_int,
            )
        })
    }

    /// Whether the last [`write_sensor`](Device::write_sensor) completed.
    pub fn write_sensor_successful(&self, device: i32, bank: i32) -> Result<()> {
        let mut h = self.handle;
        check(unsafe { ffi::neoradio2_write_sensor_successful(&mut h, device, bank) })
    }
}

impl Drop for Device {
    fn drop(&mut self) {
        unsafe { ffi::neoradio2_close(&mut self.handle) };
    }
}
