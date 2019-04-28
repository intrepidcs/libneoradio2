<p align="center">
<img src="https://www.intrepidcs.com/wp-content/uploads/2018/03/IntrepidCS_logo.png" width="350"/>

</p>


# RAD-IO2 Python API

The RAD-IO2 series is a family of ruggedized products that provide an isolated analog, digital or temperature interface to a PC via the PC’s USB port. These tools can also be paired with Intrepid products that include a USB port such as neoVI ION, neoVI FIRE 2, RAD-Galaxy, and RAD-Gigalog. In addition, the RAD-IO2-CANHUB can power and convert the native UART signal to CAN or CAN FD for use in any CAN device.

The RAD-IO2 family communicates on an open source UART based serial communication protocol. Up to eight devices can be daisy chained. The chain length is limited by current supplied to the chain through USB. All RAD-IO2 devices have input to output isolation, and 2.5kV isolation between each of the eight banks. Bank to bank isolation is important because it allows the common mode voltage of each input signal to be different than the other channels in other banks. (This is a major source of measurement error and can damage to the product.) Additionally, noise on one channel will not affect other channels.

The list of RAD-IO2 devices supported by the API are as follows:

* [RAD-IO2-TC](https://www.intrepidcs.com/products/analog-daq-devices/rad-io2-series/)
* [RAD-IO2-AIN](https://www.intrepidcs.com/products/analog-daq-devices/rad-io2-series/)
* [RAD-IO2-AOUT](https://www.intrepidcs.com/products/analog-daq-devices/rad-io2-series/)
* [RAD-IO2-PWRRLY](https://www.intrepidcs.com/products/analog-daq-devices/rad-io2-series/)
* [RAD-IO2-DIO](https://www.intrepidcs.com/products/analog-daq-devices/rad-io2-series/)
* [RAD-IO2-CANHUB](https://www.intrepidcs.com/products/analog-daq-devices/rad-io2-series/)
* [RAD-IO2-BADGE](https://www.intrepidcs.com/products/analog-daq-devices/rad-io2-series/)



### Prerequisities
<a href="https://www.python.org/downloads/">Python 3.5 or Higher running on your local system </a> <br>

## Installation
Install from PyPi using [pip](http://www.pip-installer.org/en/latest/), a
package manager for Python.

```
pip install neoradio2
```
Don't have pip installed? Try installing it, by running this from the command
line:
```
$ curl https://raw.github.com/pypa/pip/master/contrib/get-pip.py | python
```

You may need to run the above commands with `sudo`.


## Getting Started

* RAD-IO2-BADGE: Demo Board For Multichannel Analog Measurement
The RAD-IO2-Badge is a demonstration platform that can measure multichannel analog input and digital output on a single low-cost device. The device mimics the RAD-IO2-AIN and RAD-IO2-PWRRLY.

* A Tour of RAD-IO2-BADGE Hardware - please [click here](docs/badge/readme/Badge_HW_GUIDE.md)
* For guide on getting started with Analog Inputs using the python API for RAD-IO2-BADGE - please [click here](docs/badge/readme/Badge_AIN_GUIDE.md)
* For guide on Digital Output using the python API for RAD-IO2-BADGE  - please [click here](docs/badge/readme/Badge_DO_GUIDE.md)


## License

This project is licensed under the BSD License - see the [LICENSE.md](LICENSE.md) file for details




