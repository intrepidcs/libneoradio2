use neoradio2::{CalType, Device};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    for info in Device::find()? {
        println!("{info:?}");
        let dev = Device::open(&info)?;
        dev.chain_identify()?;
        dev.app_start(0, 0xFF)?;
        dev.request_sensor_data(0, 0xFF, CalType::Enabled)?;
        for bank in 0..8 {
            if let Ok(v) = dev.read_sensor_float(0, bank) {
                println!("  bank {bank}: {v}");
            }
        }
    }
    Ok(())
}
