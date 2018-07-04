# -*- coding: utf-8 -*-
"""
author:willm2

"""

#!/usr/bin/env python3

import tkinter as tk
import DeviceFunctions as DF #functions that use the picolog driver (linux)

#print("before run pico1 is %s and DF.pico1 is %s" %(Pico1,DF.Pico1))
class Application(tk.Frame):
    global Pico1#handle of device
    def __init__(self, master=None):

        tk.Frame.__init__(self, master)
        #some attributes
        
        print("Init")
        #create Layout
        self.grid()  

        self.createWidgets()
        
    def __enter__(self):
        print("Enter")
        self.DeviceOpenCheck()
        return self
    def __exit__(self,exc_type, exc_val,exc_tb) :#cleaning up
        global Pico1
        print("Exiting" )
        DF.CloseDevice(Pico1)
    def createWidgets(self):
        print("Creating widgets")
        global Pico1
        #making the window resizable
        top = self.winfo_toplevel()
        top.rowconfigure(0, weight=1)
        top.columnconfigure(0, weight=1)
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)
        #buttons
        #self.quitButton = tk.Button(self, text='Open device', command=lambda:DF.Printer(1), fg='blue', relief="sunken")#quit

        #self.quitButton.grid(row=0, column=0, sticky=tk.N+tk.S+tk.E+tk.W)#args needed for resizing
        #self.quitButton2 = tk.Button(self, text='Monkey sees,', command=lambda:self.Monkey(), fg='red')#quit
        #
        #self.OpenDeviceButton = tk.Button(self, text='ConnectDevice', command=lambda:DF.OpenDevice(), fg='black')
        #self.OpenDeviceButton.grid(row=0, column=0, sticky=tk.N+tk.S+tk.E+tk.W)
        #self.CloseDeviceButton = tk.Button(self, text='CloseDevice', command=lambda:DF.CloseDevice(Pico1), fg='black')
        #self.CloseDeviceButton.grid(row=0, column=1, sticky=tk.N+tk.S+tk.E+tk.W)

        #self.quitButton2.grid(row=1, column=1, sticky=tk.N+tk.S+tk.E+tk.W)
        #canvas
        self.ConnectedDeviceButton_text = tk.StringVar()
        self.ConnectedDeviceButton_text.set("Device")
        self.ConnectedDeviceButton = tk.Button(self, textvariable=self.ConnectedDeviceButton_text, command=lambda:self.ClickInfo(), fg='black')
        self.ConnectedDeviceButton.grid(row=0, column=0, sticky=tk.N+tk.S+tk.E+tk.W)
        #self.OpenCheck = tk.Canvas(self, width=250, height=15,bg="grey",relief="raised")#
        #self.OpenCheck.grid(row=1, column=0, sticky=tk.N+tk.S+tk.E+tk.W)
    def ClickInfo(self):
        toplevel =tk.Toplevel()
        toplevel.title("Info about PicoLog")
        InfoText = DF.PrintInfo()
        label1 = tk.Label(toplevel, text=InfoText)#, height =0, width =100)
        label1.pack()
    #some test function
    def Monkey(self):#dummy function
        #if quitButton2["text"] == 'Monkey sees,':
        #self.text='Monkey does.'
        #tk.update()
        print("Monkey does.")
    def DeviceOpenCheck(self):#checks if a device is connected and set button to green
        global Pico1
        Pico1 = DF.OpenDevice()
        
        if Pico1==1:#green button if device connected/red if disconnected
            self.ConnectedDeviceButton_text.set("device %s connected"%Pico1)#,height=50
            #self.ConnectedDeviceButton.bg("green")
            self.ConnectedDeviceButton.config(bg="green")
        else:
           #self.OpenCheck.bg("red")
           self.ConnectedDeviceButton_text.set("ERROR %s, no device connected"%Pico1)
           self.ConnectedDeviceButton.config(bg="red")
           #self.OpenCheck.create_text(25,5,anchor="w",text="ERROR %s, no device connected"%Pico1,fill="red") 
       
#run the frame/application
with Application() as app:
#app = Application()
    app.mainloop()
   
