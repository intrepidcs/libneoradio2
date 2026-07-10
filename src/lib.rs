//! Rust bindings for the Intrepid Control Systems neoRAD-IO2 device library.

pub mod ffi;
mod error;

pub use error::{Error, Result};
pub(crate) use error::check;
