import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style

import neoradio2
import threading
import time

import numpy as np
import scipy.signal

style.use('seaborn-poster')

values = np.array([])

fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)
title = "neoRAD-IO2 Bank 1"

def sample_sensor(event):
    print("Sampling sensor...")
    global title
    global values
    devs = neoradio2.find_devices()
    handle = neoradio2.open(devs[0])
    title = "{} {} Bank 1".format(devs[0].name, devs[0].serial_str)
    # Make sure we are in application firmware
    for x in range(8):
        neoradio2.app_start(handle, 0, x)
    while True:
        samples = []
        #for x in range(3):
        #    samples.append(neoradio2.read_sensor_float(handle, 0, 0)*1.8+32)
        #    time.sleep(0.1)
        
        #values = np.append(values, scipy.signal.savgol_filter(np.array(samples), 3, 1))
        #values = np.append(values, sum(samples)/len(samples))
        values = np.append(values, neoradio2.read_sensor_float(handle, 0, 0)*1.8+32)
        #if len(values) > 600:
        #    values.pop(0)
        
        if len(values) > 3:
            values = scipy.signal.savgol_filter(values, 3, 1)
        time.sleep(0.100)
        if event.is_set():
            break
    neoradio2.close(handle)


def animate(i):
    ax1.clear()
    ax1.set_title(title)
    ax1.set_xlabel('Sample Time')
    ax1.set_ylabel('Temperature (F)')
    plt.ylim(ymin=50, ymax=100)
    line, = ax1.plot([x for x in range(len(values))], values)


thread_event = threading.Event()
thread = threading.Thread(target=sample_sensor, args=(thread_event,))
print("Starting Thread...")
thread.start()

print("Starting Animation")
ani = animation.FuncAnimation(fig, animate, interval=250)
print("Showing Plot")
plt.show()

thread_event.set()
thread.join()