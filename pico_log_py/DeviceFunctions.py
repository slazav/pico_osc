#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Apr 17 17:28:13 2018

@author: willm2
For this
"""

import ctypes as ct
Pico1 = 0#global handle of the opened device
ps = ct.CDLL("libpicohrdl.so.2")#open driver api for Picolog ADC24

def OpenDevice():
    global Pico1#gloabal variable which contains the handle of the picolog. Typically Pico1 = 1
    global ps
    Pico1 = ps.HRDLOpenUnit()
    if Pico1 >= 1:
        print("Device opened with handle %s" % Pico1)
    elif Pico1 == 0:
        print("No device found.")
    elif Pico1 == -1:
        print("Failed to open device.")
    return Pico1

def PrintInfo():#just prints spme information of the picolog
    global Pico1
    InfoOption =["Driver Version: ", "USB Version: ", "Hardware Version: ","Variant Info: ",
                 "Batch and Serial: ", "Calibration Date: ", "Kernel driver Version: " ]
    InfoString = "Infos:\n"
    Pico1Info = ct.create_string_buffer(10)
    for n in range(0,6):#getting the info from the driver function
        ps.HRDLGetUnitInfo(Pico1,Pico1Info,8,n)
        InfoString += InfoOption[n]+Pico1Info.value.decode("utf-8")+"\n"
    return InfoString

def CloseDevice(device):#closes the device
    global Pico1
    global ps
    Status = ps.HRDLCloseUnit(device)
    #print(Pico1)
    if Status == 1:
        print("Device %s closed."%Pico1)
    elif Status== 0:
        print("No device closed.")
    elif Status == -1:
        print("All devices closed.")

def Printer(self):#some dummy function
    print(self)
    #some dummy function calls of the driver
    #    rdy = ps.HRDLReady(Pico1)
    #    ps.HRDLGetUnitInfo(Pico1,Pico1Info,8,4)
    #print(repr(Pico1Info.raw))
    #    SetChannel = ps.HRDLSetAnalogInChannel(Pico1,1,1,0,1)
    #    NoOfChannels = ps.HRDLGetNumberOfEnabledChannels()



