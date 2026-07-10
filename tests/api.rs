// No-hardware tests: these must pass with nothing plugged in.
#[test]
fn find_returns_a_vec_without_error() {
    // With no device connected this returns an empty Vec (never an error).
    let list = neoradio2::Device::find().expect("find should not error");
    // Length is >= 0 by construction; assert the call succeeded and is bounded.
    assert!(list.len() <= 8);
}
