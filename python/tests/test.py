import neoradio2



assert neoradio2.__version__ == '0.0.1'

count, devices = neoradio2.FindDevices()
print("Found %d device(s):" % count)
for i, device in enumerate(devices):
    print('\t', i+1, device)

assert count > 0
assert len(devices) > 0
assert repr(devices[0]) == '<neoradio2.USBDevice object IA0001>'
