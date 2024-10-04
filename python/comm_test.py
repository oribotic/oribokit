import serial
import serial.tools.list_ports
import threading
import time
import math
import rtmidi

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
def show_avail_ports():

    print('Available serial ports:')
    for port, desc, hwid in sorted(serial.tools.list_ports.comports()):
        print(f'{port} - {desc} - {hwid}')

# -----------------------------------------------
def showAvailCommands():

    print('Enter the number for the command to send:')
    print('  1  ... calibrate')
    print('  2  ... set hard')
    print('  3  ... set bend low')
    print('  4  ... set bend high')
    print('  5  ... set low hard soft')
    print('  6  ... set soft')
    print('  7  ... get notes')
    print('  8  ... set root')
    print('  9  ... set mode')
    print('  10 ... set scale')
    print('  11 ... set CDT')
    print('  12 ... set interval')
    print('  13 ... set FFI')
    print('  14 ... set SFI')

# -----------------------------------------------
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

    addr = ''
    type = ','
    data = '\b0'

    if cmd == 1:
        addr = '/calibrate'
    elif cmd == 2:
        addr = '/setKey/hard'
    elif cmd == 3:
        addr = '/setKey/belo'
    elif cmd == 4:
        addr = '/setKey/behi'
    elif cmd == 5:
        addr = '/setKey/loHS'
    elif cmd == 6:
        addr = '/setKey/soft'
    elif cmd == 7:
        addr = '/getNotes'

    elif cmd == 8:
        addr = '/setRoot'
        type += 'i'
        data = inputToData('set root:')

    elif cmd == 9:
        addr = '/setMode'
        type += 'i'
        data = inputToData('set mode:')

    elif cmd == 10:
        addr = '/setScale'
        type += 'i'
        data = inputToData('set scale:')

    elif cmd == 11:
        addr = '/setCDT'
        type += 'i'
        data = inputToData('set CDT:')

    elif cmd == 12:
        addr = '/setInterval'
        type += 'i'
        data = inputToData('set interval:')

    elif cmd == 13:
        addr = '/setFFIFilter'
        type += 'i'
        data = inputToData('set FFI:')

    elif cmd == 14:
        addr = '/setSFIFilter'
        type += 'i'
        data = inputToData('set SFI:')

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
        data = (data + '\0').encode()

    # data = bytearray(data)
    # data.reverse()

    msg = addr.encode() + type.encode() + data
    # print(f'Total message size: {len(msg)} bytes')
    return msg

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
def main():

    MODE_OSC = 1
    MODE_MIDI = 2

    doWork = True
    mode = -1
    while mode != MODE_OSC and mode != MODE_MIDI:
        try:
            mode = int(input('Mode (1 = OSC, 2 = MIDI): '))
        except ValueError:
            print('[WARN] Invalid mode input...')
            pass
        except KeyboardInterrupt:
            doWork = False
            break

    if mode == MODE_OSC:

        # check serial port argument
        serPort = input('Serial Port: ')
        if len(serPort) == 0:
            print('[ERR] No serial port specified!')
            show_avail_ports()
            exit()
        else:
            print(f'Using serial port: {serPort}')

        # try to connect
        bgTask = SerialResetInputTask()
        try:
            ser = serial.Serial(serPort, write_timeout=0.5, timeout=1.0, rtscts=False)
            bgTask.setPort(ser)
            bgTask.start()
        except Exception as e:
            print(f'[ERR] Failed to open serial port {serPort} ({e})!')
            exit()

        print(f'Connected to: {serPort}')

    elif mode == MODE_MIDI:

        midiout = rtmidi.MidiOut()
        available_ports = midiout.get_ports()

        oriMidiPort = 0
        for port in available_ports:
            if 'Oricordion' in port:
                print(f'Found MIDI device: {oriMidiPort}')
                break
            oriMidiPort += 1

        if oriMidiPort == len(available_ports):
            print(f'[ERR] failed to find Oricordion MIDI!')
            doWork = False
    
        midiout.open_port(oriMidiPort)
        print(f'MIDI opened')

    else:
        doWork = False
        print('[ERR] unkwnon mode!')

    while doWork:
        try:

            showAvailCommands()
            cmd = int(input())

            # handle command and setup data to be sent
            txData = None
            if mode == MODE_OSC:
                txData = handleCmd_OSC(cmd)

                # write
                if txData != None:
                    print(f'Sending command: {txData}')
                    ser.write(txData)
                    ser.flush()

            elif mode == MODE_MIDI:
                txData = handleCmd_MIDI(cmd)
                
                # write
                if txData != None:
                    print(f'Sending command: {txData}')
                    midiout.send_message(txData)


        except ValueError:
            print('[WARN] Invalid input, try again...')
            pass

        except KeyboardInterrupt:
            break

        except serial.serialutil.SerialTimeoutException:
            print('[WARN] Write timeout, try again...')
            pass

    if mode == MODE_OSC:
        bgTask.setPort(None)
        bgTask.join()

        ser.close()
        print(f'Closed {serPort}')

    elif mode == MODE_MIDI:

        midiout.close_port()
        midiout.delete()
        print(f'MIDI closed')

# -----------------------------------------------
if __name__ == '__main__':
    main()