import serial
import datetime
import math
import struct
import time
import threading
from serial.tools import list_ports
import sliplib
import sys
import platform
import Adafruit_MPR121.MPR121 as MPR121


# -----------------------------------------------
class SerialResetInputTask(threading.Thread):

    port = None

    def run(self,*args,**kwargs):
        while True:
            if self.port == None or not self.port.is_open:
                break

            self.port.reset_input_buffer()
            time.sleep(0.1)

    def setPort(self, port):
        self.port = port

# -----------------------------------------------
def get_avail_ports():
    portlist = []
    port = list(list_ports.comports())
    for p in port:
        #print (platform.system())
        if platform.system() == "Darwin":
            if "usb" in p.device:
                portlist.append(p.device)
        else:
            portlist.append(p.device)
    return portlist

# -----------------------------------------------
def showAvailCommands():
    print('Enter the number for the command to send:')
    print('  1  ... activate bot 1')
    print('  2  ... activate bot 2')
    print('  3  ... activate bot 3')
    print('  4  ... bloom all bots')

def inputToData(prompt, midiMode=False):

    inputNum = None
    while True:
        try:
            inputStr = input(prompt +  ' ')
            inputNum = int(inputStr)
            break
        except ValueError:
            print('[ERR] Not a number!')
            continue
        except Exception as e:
            print('[ERR] Failed to parse input ({e})!')
            break

    if midiMode:
        return inputNum

    bNeeded = math.ceil(math.log(inputNum, 256))
    return inputNum.to_bytes(length=bNeeded, byteorder='little')

# -----------------------------------------------
def handleCmd_OSC(cmd):
    
    END = 0xc0

    addr = ''
    type = ','
    data = '\b0'

    if cmd == 1:
        addr = '/cap/1'
    elif cmd == 2:
        addr = '/cap/2'
    elif cmd == 3:
        addr = '/cap/3'
    elif cmd == 4:
        addr = '/bloom'
    
    # 4 bytesa ligned address field
    addr += '\0'
    while len(addr) % 4 != 0:
        addr += '\0'

    # 4 bytesa ligned type field
    type += '\0'
    while len(type) % 4 != 0:
        type += '\0'

    # 4 bytes aligned data field
    while len(data) % 4 != 0:
        data = (data + '\b0').encode()

    # data = bytearray(data)
    # data.reverse()   
    
    

    msg = addr.encode() + type.encode() + data 
    
    slip = sliplib.encode(msg)
    print (msg)
    print (slip)
    # print(f'Total message size: {len(msg)} bytes')
    return slip

# -----------------------------------------------
def handleCmd_MIDI(cmd):

    msg = []
    # msg.append(0xf0)
    # msg.append(0x0b)

    channel = 0
    control = 0
    value = 0

    if cmd == 1:
        # calibrate
        control = 111
        value = 100
    elif cmd == 2:
        # hard
        control = 111
        value = 0
    elif cmd == 3:
        # belo
        control = 111
        value = 10
    elif cmd == 4:
        # behi
        control = 111
        value = 15
    elif cmd == 5:
        # loHS
        control = 111
        value = 20
    elif cmd == 6:
        # soft
        control = 111
        value = 5
    elif cmd == 7:
        # getNotes
        print('getNotes - not implemented')
        return None
    elif cmd == 8:
        # setRoot
        control = 109
        value = inputToData('set root:', True)

    elif cmd == 9:
        # setMode
        control = 110
        value = inputToData('set mode:', True)

    elif cmd == 10:
        # setScale
        control = 108
        value = inputToData('set scale:', True)

    elif cmd == 11:
        # setCDT
        control = 115
        value = inputToData('set CDT:', True)

    elif cmd == 12:
        # setInterval
        control = 114
        value = inputToData('set interval:', True)

    elif cmd == 13:
        # setFFIFilter
        control = 112
        value = inputToData('set FFI:', True)

    elif cmd == 14:
        # setSFIFilter
        control = 113
        value = inputToData('set SFI:', True)
    
    else:
        return None

    msg.append(0xb0 | (0x0F & channel))
    msg.append(control & 0x7F)
    msg.append(value & 0x7F)

    # print(f'Total message size: {len(msg)} bytes -> {msg}')
    return msg

# -----------------------------------------------
def bloom():
    allports = get_avail_ports()
    print (allports)
    for port in allports:
        
        try:
            bgTask = SerialResetInputTask()
            ser = serial.Serial(port, write_timeout=0.5, timeout=1.0, rtscts=False)
            bgTask.setPort(ser)
            bgTask.start()
        except Exception as e:
            print(f'[ERR] Failed to open serial port {serPort} ({e})!')
            exit()
        try: 
            txData = handleCmd_OSC(4) # send bloom command to all
            print(f'Sending command: {txData}')
            ser.write(txData)
            ser.flush()
            
            bgTask.setPort(None)
            bgTask.join()
            ser.close()
            print(f'Closed {port}')
        except KeyboardInterrupt:
            break

        except serial.serialutil.SerialTimeoutException:
            print('[WARN] Write timeout, try again...')
            pass


print('ORIBOKIT SERIAL CAP SENSE')

# Create MPR121 instance.
cap = MPR121.MPR121()

# Initialize communication with MPR121 using default I2C bus of device, and
# default I2C address (0x5A).  On BeagleBone Black will default to I2C bus 0.
if not cap.begin(address=0x5C):
    print('Error initializing MPR121.  Check your wiring!')
    sys.exit(1)

# Alternatively, specify a custom I2C address such as 0x5B (ADDR tied to 3.3V),
# 0x5C (ADDR tied to SDA), or 0x5D (ADDR tied to SCL).
#cap.begin(address=0x5B)

# Also you can specify an optional I2C bus with the bus keyword parameter.
cap.begin(address=0x5C)

# Main loop to print a message every time a pin is touched.
print('Press Ctrl-C to quit.')
last_touched = cap.touched()
delay = 500

while True:
    current_touched = cap.touched()
    # Check each pin's last and current state to see if it was pressed or released.
    #print (current_touched)
    
    i = 1
    pin_bit = 1 << i
    # First check if transitioned from not touched to touched.
    if current_touched & pin_bit and not last_touched & pin_bit:
        print('{0} touched!'.format(i))
        bloom()
    
    # for i in range(12):
    #     # Each pin is represented by a bit in the touched value.  A value of 1
    #     # means the pin is being touched, and 0 means it is not being touched.
    #     pin_bit = 1 << i
    #     # First check if transitioned from not touched to touched.
    #     if current_touched & pin_bit and not last_touched & pin_bit:
    #         print('{0} touched!'.format(i))
            
        # Next check if transitioned from touched to not touched.
        # if not current_touched & pin_bit and last_touched & pin_bit:
        #     print('{0} released!'.format(i))
    
    # Update last state and wait a short period before repeating.
    last_touched = current_touched
    time.sleep(0.1)


