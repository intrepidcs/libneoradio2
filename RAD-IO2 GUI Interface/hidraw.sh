#!/bin/bash

cd /etc/udev/rules.d
touch 99-hidraw-permissions.rules
echo 'KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0666"' >> 99-hidraw-permissions.rules
echo "hid rules created in /etc/udev/rules.d/"