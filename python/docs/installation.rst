============
Installation
============

``neoradio2`` publishes pre-built wheels for Windows, macOS, and Linux
(CPython 3.9–3.14), so in most cases installation is simply:

.. code-block:: console

    pip install neoradio2

**Windows:** ``pip.exe`` is usually located under the ``Scripts`` directory of
your Python installation.

Linux — udev rules
==================

On Linux the udev rules must be installed so the device is accessible without
root. Download `99-intrepidcs.rules
<https://raw.githubusercontent.com/intrepidcs/libneoradio2/master/99-intrepidcs.rules>`_
and install it:

.. code-block:: console

    sudo cp 99-intrepidcs.rules /etc/udev/rules.d/
    sudo udevadm control --reload-rules && sudo udevadm trigger

Then add your user to the ``users`` group and log out/in for it to take effect:

.. code-block:: console

    sudo usermod -aG users $USER

If you would rather write the rule by hand, the neoRAD-IO2 uses USB vendor id
``093c`` and product id ``1300``:

.. code-block:: none

    SUBSYSTEM=="hidraw", ATTRS{idVendor}=="093c", ATTRS{idProduct}=="1300", GROUP="users", MODE="0666"
    KERNEL=="hidraw*", ATTRS{idVendor}=="093c", ATTRS{idProduct}=="1300", GROUP="users", MODE="0666"

Re-plug the device after installing the rules.

Building from source
====================

If no wheel is available for your platform, ``pip`` builds from source. The
repository uses git submodules, so a source build needs a CMake toolchain and
a compiler (MSVC on Windows, GCC/Clang elsewhere). See the `README
<https://github.com/intrepidcs/libneoradio2#build-from-source>`_ for details.
