# neoradio2 Python library

![Intrepid Control Systems, Inc.](https://raw.githubusercontent.com/intrepidcs/libneoradio2/master/IntrepidCS_logo.png)

Python bindings for [libneoradio2](https://github.com/intrepidcs/libneoradio2) —
the library for communicating with the Intrepid Control Systems neoRAD-IO2
family of ruggedized, isolated analog / digital / temperature DAQ devices.

## Documentation

https://intrepidcs.github.io/libneoradio2/python/docs/html/

## Installation

```
pip install neoradio2
```

**Windows:** `pip.exe` is usually located under the `Scripts` directory of your
Python installation.

### Linux — udev rules

So the device is accessible without root, install the udev rules. Download
[`99-intrepidcs.rules`](https://raw.githubusercontent.com/intrepidcs/libneoradio2/master/99-intrepidcs.rules)
and install it:

```
sudo cp 99-intrepidcs.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger
```

Then add your user to the `users` group and re-plug the device:

```
sudo usermod -aG users $USER
```

(log out and back in for the group change to take effect). If you'd rather write
the rule by hand, the neoRAD-IO2 uses USB VID `093c`, PID `1300`:

```
SUBSYSTEM=="hidraw", ATTRS{idVendor}=="093c", ATTRS{idProduct}=="1300", GROUP="users", MODE="0666"
KERNEL=="hidraw*", ATTRS{idVendor}=="093c", ATTRS{idProduct}=="1300", GROUP="users", MODE="0666"
```

## Quick example

```python
#!/usr/bin/env python3
import neoradio2

if __name__ == "__main__":
    # Open and close all the attached neoRAD-IO2 devices
    for device in neoradio2.find():
        print(device)
        handle = neoradio2.open(device)
        # Do stuff here
        neoradio2.close(handle)
```

## neoRAD-IO2 Product Line

The neoRAD-IO2 series is a family of ruggedized products that provide an
isolated analog, digital or temperature interface to a PC via the PC's USB port.
These tools can also be paired with Intrepid products that include a USB port
such as neoVI ION, neoVI FIRE 2, RAD-Galaxy, and RAD-Gigalog. In addition, the
neoRAD-IO2-CANHUB can power and convert the native UART signal to CAN or CAN FD
for use in any CAN device.

The neoRAD-IO2 family communicates on an open source UART based serial
communication protocol. Up to eight devices can be daisy chained. The chain
length is limited by current supplied to the chain through USB. All neoRAD-IO2
devices have input to output isolation, and 2.5 kV isolation between each of the
eight banks. For more details please visit
https://www.intrepidcs.com/products/analog-daq-devices/rad-io2-series/

## neoRAD-IO2-BADGE Quick Start

neoRAD-IO2-BADGE is a demo board for multi-channel analog measurement. The device
mimics the neoRAD-IO2-AIN and neoRAD-IO2-PWRRLY products. Examples are
[here](https://github.com/intrepidcs/libneoradio2/tree/master/python/example/badge).

## License — MIT

![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)

Copyright © 2019 Intrepid Control Systems, Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
