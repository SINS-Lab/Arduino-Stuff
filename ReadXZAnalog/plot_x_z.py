import serial
import time
import math
import csv
import sys
import random
import matplotlib
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
import matplotlib.animation as anim                # Updating said image
import numpy as np
import multiprocessing                             # Reading async from plotting
from multiprocessing import Queue                  # multiprocessing queue

x = -1
z = -1
lastX = -1
lastZ = -1
ani = None
plot = None
ax = None
fig = None

plot_window = 1024
x_scale = 1

x_var = np.array([])
y_var = np.array([])

def check_ser(q):
    ser = serial.Serial('COM4',115200)
    ser.flushInput()
    while True:
        ser_bytes = ser.readline()
        try:
            decoded_bytes = ser_bytes.decode("utf-8").rstrip()
            decoded_bytes = decoded_bytes.split(',')
            x = float(decoded_bytes[0]) * x_scale
            z = float(decoded_bytes[1])
            ser.flushInput()
            q.put((x,z))
            # print("Read: {}".format(decoded_bytes))
            time.sleep(0.01)
        except KeyboardInterrupt:
            break
        except:
            continue
    ser.flushInput()
    print("Closing Serial Connection")
    ser.close()

def impact_plot(q):
    global ax
    global fig
    global ani
    global plot
    plt.ion()
    fig, ax = plt.subplots()
    scat, = ax.plot(x_var, y_var)
    plot = scat
    ani = anim.FuncAnimation(fig, update_plot, fargs=(q,), interval=50)

def update_plot(*args):
    q = args[1]
    global x_var
    global y_var
    global plot_window
    global ax
    global fig
    global plot
    x = -1
    z = -1
    has = False

    # Doing it this way will just clear the queue, and pull the last value.
    while q.qsize() > 0:
        try:
            result = q.get_nowait()
            if result != '':
                x = result[0]
                z = result[1]
                has = True
        except:
            has = False

    if not has:
        return

    if(len(x_var)>plot_window):
        index = -1
        dx_min = 1e30
        i = 0
        for x_0 in x_var:
            dx = abs(x_0 - x)
            if dx < dx_min:
                dx_min = dx
                index = i
            i = i + 1
        if(index != -1):
            x_var[index] = x
            y_var[index] = z
        else:
            x_var = np.append(x_var, x)
            y_var = np.append(y_var, z)

            x_var = x_var[1:plot_window+1]
            y_var = y_var[1:plot_window+1]
    else:
        x_var = np.append(x_var, x)
        y_var = np.append(y_var, z)

    x_var,y_var = zip(*sorted(zip(x_var, y_var)))

    plot.set_xdata(x_var)
    plot.set_ydata(y_var)
    ax.relim()
    ax.autoscale_view()
    fig.canvas.draw()
    fig.canvas.flush_events()


if __name__ == '__main__':
    # Start the queue for handling multiprocessing
    q = multiprocessing.Queue()

    read_file = multiprocessing.Process(None, check_ser, args=(q,))
    read_file.start()

    impact_plot(q)

    input('')