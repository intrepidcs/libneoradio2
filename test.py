from ctypes import *
import os

path = r"E:\development\neoRADIO2\libneoradio2\libneoradio2\Debug\\"
dll_name = 'libneoradio2.dll'

os.chdir(path)
dll = WinDLL(path + dll_name)
input(os.getpid())

dll.find_hid_devices()

input("Finished...")