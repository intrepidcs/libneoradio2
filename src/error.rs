use std::os::raw::c_int;

/// Errors returned by the neoRAD-IO2 API.
#[derive(Debug, Clone, PartialEq, Eq)]
#[non_exhaustive]
pub enum Error {
    /// General failure (`NEORADIO2_FAILURE` / `NEORADIO2_ERR_FAILURE`).
    Failure,
    /// Non-blocking call would block (`NEORADIO2_ERR_WBLOCK`).
    WouldBlock,
    /// Non-blocking operation still in progress (`NEORADIO2_ERR_INPROGRESS`).
    InProgress,
    /// A Rust string contained an interior NUL byte.
    NulByte,
}

impl std::fmt::Display for Error {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let s = match self {
            Error::Failure => "neoRAD-IO2 operation failed",
            Error::WouldBlock => "operation would block (non-blocking mode)",
            Error::InProgress => "operation in progress (non-blocking mode)",
            Error::NulByte => "string contained an interior NUL byte",
        };
        f.write_str(s)
    }
}

impl std::error::Error for Error {}

/// Crate result type.
pub type Result<T> = std::result::Result<T, Error>;

/// Map a C return code to a `Result`.
pub(crate) fn check(code: c_int) -> Result<()> {
    match code {
        0 => Ok(()),
        2 => Err(Error::WouldBlock),
        3 => Err(Error::InProgress),
        _ => Err(Error::Failure),
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn maps_return_codes() {
        assert!(check(0).is_ok());
        assert_eq!(check(1).unwrap_err(), Error::Failure);
        assert_eq!(check(2).unwrap_err(), Error::WouldBlock);
        assert_eq!(check(3).unwrap_err(), Error::InProgress);
        assert_eq!(check(4).unwrap_err(), Error::Failure);
        assert_eq!(check(99).unwrap_err(), Error::Failure);
    }
}
