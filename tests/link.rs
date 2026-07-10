// Proves the C library links: is_blocking() returns an int without crashing.
#[test]
fn library_links_and_calls() {
    let blocking = unsafe { neoradio2::ffi::neoradio2_is_blocking() };
    assert!(
        blocking == 0 || blocking == 1,
        "unexpected value: {blocking}"
    );
}
