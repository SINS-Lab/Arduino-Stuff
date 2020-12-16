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

ser_port = 'COM4'
ser_rate = 115200

plot_window = 1024
x_scale = 1
z_scale = 1

x_var = np.array([])
y_var = np.array([])

def check_ser(q):
    global x_scale
    global z_scale
    global ser_port
    global ser_rate
    ser = serial.Serial(ser_port,ser_rate)
    ser.flushInput()
    while True:
        ser_bytes = ser.readline()
        try:
            decoded_bytes = ser_bytes.decode("utf-8").rstrip()
            decoded_bytes = decoded_bytes.split(',')
            x = float(decoded_bytes[0]) * x_scale
            z = float(decoded_bytes[1]) * z_scale
            ser.flushInput()
            q.put((x,z))
            # delay 10ms to give time for the KeyboardInterrupt to fire
            time.sleep(0.001)
        except KeyboardInterrupt:
            # In this case, we want to close the serial connection and exit
            # so we break out of the while True loop here.
            break
        except:
            # Other serial errors we will just ignore, it is most likely
            # something went wrong with the data sent, so we ignore it.
            continue
    ser.flushInput()
    print("Closing Serial Connection")
    ser.close()

def make_plot(q):
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
    # These values can occur much faster than the update time of the plot,
    # so we only care about 1 of them anyway.
    while q.qsize() > 0:
        try:
            result = q.get_nowait()
            if result != '' and len(result) == 2:
                x = result[0]
                z = result[1]
                has = True
        except:
            has = False

    if not has:
        return

    if(len(x_var) > plot_window):
    # If we already have plot_window points, we need
    # to decide which to remove to add the new one
        index = -1
        dx_min = 1e30
        i = 0
        # Here we look for the closest value
        # to the x being read in
        for x_0 in x_var:
            dx = abs(x_0 - x)
            if dx < dx_min:
                dx_min = dx
                index = i
            i = i + 1
            
        if(index != -1):
        # If we found a value above, we will replace it
            x_var[index] = x
            y_var[index] = z
        else:
        # Otherwise append to the end of the list, and
        # delete the earlies entry in the list
            x_var = np.append(x_var, x)
            y_var = np.append(y_var, z)

            x_var = x_var[1:plot_window+1]
            y_var = y_var[1:plot_window+1]
    else:
    # Otherwise we just add the new one
        x_var = np.append(x_var, x)
        y_var = np.append(y_var, z)

    # This sorts the lists by x_var, so that the plot
    # will be in the correct order.
    x_var,y_var = zip(*sorted(zip(x_var, y_var)))

    # Updates the plot values
    plot.set_xdata(x_var)
    plot.set_ydata(y_var)
    # Updates the view window
    ax.relim()
    ax.autoscale_view()
    # Updates the actual figure
    fig.canvas.draw()
    fig.canvas.flush_events()


if __name__ == '__main__':
    # Start the queue for handling multiprocessing
    q = multiprocessing.Queue()

    read_serial = multiprocessing.Process(None, check_ser, args=(q,))
    read_serial.start()

    make_plot(q)

    input('')
