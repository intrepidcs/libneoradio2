use std::os::raw::{c_char, c_int, c_uint};
use crate::{check, ffi, Result};

/// The raw C handle type. `neoradio2_handle` is `#define neoradio2_handle long`
/// in the C headers, so bindgen inlines it as `c_long` rather than emitting an
/// alias; we name it here for readability within this crate.
pub(crate) type Handle = std::os::raw::c_long;

const MAX_DEVS: usize = ffi::NEORADIO2_MAX_DEVS as usize;

fn cstr_to_string(buf: &[c_char]) -> String {
    let bytes: Vec<u8> = buf.iter().take_while(|&&c| c != 0).map(|&c| c as u8).collect();
    String::from_utf8_lossy(&bytes).into_owned()
}

/// Information about a discovered neoRAD-IO2 USB device.
#[derive(Clone)]
pub struct DeviceInfo(pub(crate) ffi::Neoradio2DeviceInfo);

impl DeviceInfo {
    /// Product name (e.g. "neoRAD-IO2-AIN").
    pub fn name(&self) -> String { cstr_to_string(&self.0.name) }
    /// Serial string (e.g. "IB0370").
    pub fn serial(&self) -> String { cstr_to_string(&self.0.serial_str) }
    /// USB vendor id.
    pub fn vendor_id(&self) -> i32 { self.0.vendor_id }
    /// USB product id.
    pub fn product_id(&self) -> i32 { self.0.product_id }
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
        let mut buf: [ffi::Neoradio2DeviceInfo; MAX_DEVS] =
            unsafe { std::mem::zeroed() };
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
        Ok(Device { handle, _not_sync: std::marker::PhantomData })
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

impl Drop for Device {
    fn drop(&mut self) {
        unsafe { ffi::neoradio2_close(&mut self.handle) };
    }
}
