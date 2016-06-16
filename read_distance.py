# -*- coding: utf-8 -*-
"""
Testing out HC-SR04 on dsPIC33f
"""
import serial
import struct
import signal
import sys
import matplotlib.pyplot as plt
import numpy as np
import time
        
ser = serial.Serial(port = '/dev/ttyUSB0', 
                    baudrate = 115200,
                    bytesize = 8,
                    parity = serial.PARITY_NONE,
                    stopbits = 1,
                    timeout = 5)

user_in = 'a'
exit_loop = False

x = np.array([])
y = np.array([])

print("q to quit. Will plot entries after leaving loop")

while (not exit_loop) :
    
    user_in = raw_input("Enter distance: ")
    
    if (user_in == 'q'):
        exit_loop = True
    else:
        #log distance
        distance = float(user_in)
        x = np.append(x, distance)
        
        #sample and log adc value
        ser.write('x')
        rcvd = ser.read(2)
        rcvd = struct.unpack('H', rcvd)
        raw_adc = rcvd[0]
        y = np.append(y, raw_adc)
    
    
    
    print("At " + str(distance) + " cm we get " + str(raw_adc))

ser.close()
plt.plot(x, y)
plt.show()   

