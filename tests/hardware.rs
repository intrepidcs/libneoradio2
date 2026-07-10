//! Runs against whatever neoRAD-IO2 device is connected. If nothing is
//! connected, the test prints a note and passes (so it is CI-safe).
use neoradio2::{CalType, Device, DeviceType};

#[test]
fn exercise_connected_device() {
    let infos = Device::find().expect("find");
    let Some(info) = infos.into_iter().next() else {
        eprintln!("no neoRAD-IO2 device connected; skipping hardware test");
        return;
    };
    eprintln!("testing {} {}", info.name(), info.serial());

    let dev = Device::open(&info).expect("open");
    assert!(dev.is_opened().expect("is_opened"));
    dev.chain_identify().expect("chain_identify");
    let count = dev.chain_count(true).expect("chain_count");
    assert!(count >= 1, "expected at least one device in the chain");

    for d in 0..count {
        dev.app_start(d, 0xFF).ok();
        let ty = dev.device_type(d, 0).expect("device_type");
        let sn = dev.serial_number(d, 0).expect("serial_number");
        let (fw_major, fw_minor) = dev.firmware_version(d, 0).expect("firmware_version");
        eprintln!(
            "  device {d}: type={:?} serial={sn} fw={fw_major}.{fw_minor}",
            DeviceType::from_raw(ty as i32)
        );
        // A sensor read is meaningful only for analog devices; ignore errors.
        if dev.request_sensor_data(d, 0x01, CalType::Enabled).is_ok() {
            if let Ok(v) = dev.read_sensor_float(d, 0) {
                eprintln!("    bank0 = {v}");
            }
        }
    }
    // Device closes on drop.
}
