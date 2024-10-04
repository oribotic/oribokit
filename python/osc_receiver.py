import serial
import datetime
import math
import struct
import time
import threading

#--------------------------------
# utility for printf with style

class style:
    RED     = '\033[91m'
    BLUE    = '\033[94m'
    CYAN    = '\033[96m'
    ENDC    = '\033[0m'
    OK      = '\033[92m[OK] '
    WARN    = '\033[93m[WARN] '
    ERR     = '\033[91m[ERR] '

def prints(txt, fmt=style.ENDC, stamp=False):
    stampStr = str('')
    if stamp:
        stampStr = datetime.now().strftime('%d-%m-%Y %H:%M:%S.%f ')
    print(f'{stampStr}{fmt}{txt}{style.ENDC}')

#--------------------------------
class SlipSerial():

    def __init__(self):
        self.serial = None
        self.timeout = 0.1

        self.packet = None
        self.packetList = []

        # self.SLIP_END = 192         # 0xc0 ... end frame
        # self.SLIP_ESC = 219         # 0xdb ... frame escape
        # self.SLIP_ESC_END = 220     # 0xdc ... transported frame end
        # self.SLIP_ESC_ESC = 221     # 0xdd ... transported frame escape

        self.SLIP_END = b'\xc0'        # end frame
        self.SLIP_ESC = b'\xdb'        # frame escape
        self.SLIP_ESC_END = b'\xdc'    # transported frame end
        self.SLIP_ESC_ESC = b'\xdd'    # transported frame escape

        self.escaped = False

    def open(self, port, baudrate=115200):
        self.close()
        failed = False

        try:
            self.serial = serial.Serial(port=port, baudrate=baudrate, timeout=self.timeout)
        except serial.SerialException:
            failed = True

        if failed or not self.serial.is_open:
            prints(f'Failed to open serial port {port}!', style.ERR)
            return False

        prints(f'Serial port {self.serial.name} successfully opened', style.OK)
        return True
        
    def close(self):
        if self.serial == None:
            return

        self.serial.close()
        self.serial = None
        prints('Serial port closed', style.OK)

    def read(self):

        try:
            rxBytes = self.serial.read_all()
            # print(rxBytes)
        except TimeoutError:
            return []
        except serial.SerialException:
            prints('Serial read error, disconnect?', style.ERR)
            exit(-1)

        # prints(rxBytes)
        return self.decode(rxBytes)

    def decode(self, data):

        # empty packet list
        self.packetList = []
        
        # handle full data
        for b in data:

            b = int.to_bytes(b, 1, 'big')

            # end marks start or end of packet
            if b == self.SLIP_END:
                if self.packet != None:
                    self.packetList.append(self.packet)
                    self.packet = None
                    continue

                self.packet = bytearray()

            # ESC is followed by ESC_END or ESC_ESC
            elif b == self.SLIP_ESC:
                self.escaped = True

            # ESC + ESC_END = END, otherwise normal byte
            elif b == self.SLIP_ESC_END:
                if self.escaped:
                    self.escaped = False
                    self.packet += self.SLIP_END

                else:
                    self.packet += b

            # ESC + ESC_ESC = ESC, otherwise normal byte
            elif b == self.SLIP_ESC_ESC:
                if self.escaped:
                    self.escaped = False
                    self.packet += self.SLIP_ESC

                else:
                    self.packet += b

            # normal byte
            else:

                # if receving normal byte after ESC, raise protocol violation
                if self.escaped:
                    self.packet = None
                    self.escaped = False
                    #raise Exception('SLIP protocol violation')
                    prints('SLIP protocol violation!', style.ERR)
                    continue

                # packet must not start with END, therfore check for NONE and 'start packet'
                if self.packet == None:
                    self.packet = bytearray()

                self.packet += b

        return self.packetList

#--------------------------------
# unpack 32-bit integer and return result + rest
def unpackInt(data):
    if len(data) < 4:
        prints('Invalid OSC integer value!', style.ERR)
        return (None, data)

    value = struct.unpack('>i', data[0:4])[0]
    data = data[4:]

    return (value, data)

#--------------------------------
# unpack 32-bit floating point and return result + rest
def unpackFloat(data):
    if len(data) < 4:
        prints('Invalid OSC float value!', style.ERR)
        return (0, data)

    value = struct.unpack('>f', data[0:4])[0]
    data = data[4:]

    return (value, data)

#--------------------------------
# unpack null terminated string and return result + rest
def unpackString(data):

    length = data.find(b'\x00')
    nextStart = int(math.ceil((length + 1) / 4.0)) * 4

    value = data[:length].decode('utf-8')
    data = data[nextStart:]

    return (value, data)

#--------------------------------
# blob starts with 32-bit integer defining blob size
def unpackBlob(data):

    length = struct.unpack('>i', data[0:4])[0]
    nextStart = int(math.ceil((length + 1) / 4.0)) * 4

    value = data[4:length]
    data = data[nextStart:]

    return (value, data)

#--------------------------------
def decodeOsc(packetList):

    oscTable = { 'i': unpackInt, 'f': unpackFloat, 's': unpackString, 'b': unpackBlob }
    msgs = []

    # check if we have something to do
    if len(packetList) == 0:
        return msgs

    for packet in packetList:

        # print(packet)

        # OSC is always multiple of 4 bytes ending/padded with 0x00
        nextStart = 0
        length = packet[nextStart:].find(b'\x00')
        addr = packet[nextStart:nextStart+length].decode('utf-8')

        nextStart += int(math.ceil((length + 1) / 4.0)) * 4
        length = packet[nextStart:].find(b'\x00')
        typeTags = packet[nextStart:nextStart+length].decode('utf-8')

        nextStart += int(math.ceil((length + 1) / 4.0)) * 4
        data = packet[nextStart:]

        # typtag always starts with a ','
        if not typeTags.startswith(','):
            prints('Invalid OSC typetag!', style.ERR)
            continue
            #return [], [], []

        oscMessage = { 
            'address': addr,
            'types': [],
            'values': []
        }

        # unpack the data
        for tag in typeTags[1:]:
            oscMessage['types'].append(tag)

            value, data = oscTable[tag](data)
            if value == None:
                continue

            oscMessage['values'].append(value)

        msgs.append(oscMessage)

    return msgs

#--------------------------------

def oscDecoderThrdWorker(port, addrFilter):

    global oscData
    global plotDataLock
    global maxSamplesPerLine
    global panelgroups
    global usePanelGroups

    prints('Read and decode thread started', style.OK)

    # initialize SLIP serial
    slip = SlipSerial()
    if not slip.open(port):
        return

    oscDecoderThrdRunningLock.acquire()
    try:
        thrdRunning = oscDecoderThrdRunning
    finally:
        oscDecoderThrdRunningLock.release()

    tmpData = { }

    startTime = None
    while thrdRunning:

        # read data and decode osc messages
        packetList = slip.read()
        msgs = decodeOsc(packetList)

        # setup start time
        now = 0.0
        if startTime == None and len(msgs) > 0:
            startTime = time.time()
        elif startTime != None:
            now = time.time() - startTime

        # save all the data (thread safe)
        plotDataLock.acquire()
        try:
            i = 0

            # print(f'msg coung: {len(msgs)}')

            for msg in msgs:
                i += 1

                # filter osc messages
                if type(addrFilter) != str:

                    found = False
                    for addr in addrFilter:
                        if addr in msg['address']:
                            found = True
                            break

                    if not found:
                        continue

                elif addrFilter not in msg['address']:
                    continue

                # check for valid data
                if len(msg['values']) == 0:
                    continue

                # print(msg)

                # check if panel group data
                if '/g' in msg['address']:
                    panel = msg['address'].removeprefix('/g/')
                    panelValue = msg['values'][0]
                elif '/h' in msg['address'] or '/l' in msg['address']:
                    panel = msg['address'] + '/' + str(msg['values'][0])
                    panelValue = msg['values'][1]
                else:
                    panel = msg['values'][0]
                    panelValue = msg['values'][1]

                key = panel

                # handle normal data
                if not usePanelGroups:

                    # initialize data if not yet there
                    if key not in oscData.keys():
                        oscData[key] = ([], [])

                    # append x and y data
                    oscData[key][0].append(now)
                    oscData[key][1].append(panelValue)

                    # make sure we have a upper limit
                    if panel in oscData.keys() and len(oscData[panel][0]) > maxSamplesPerLine:
                        oscData[key][0].pop(0)
                        oscData[key][1].pop(0)

                # handle panelgroup calculated in script
                else:

                    # loop through each group (different patterns)
                    for group in panelgroups:

                        collumn = 0

                        key = None

                        # loop through 'collumns'
                        for pattern in group['pattern']:

                            # cheack each bit
                            for bit in range(0, 8):

                                # if bit is set
                                if pattern & (1 << bit):

                                    # if the current data matches the panel from the pattern
                                    e = collumn * 8 + bit
                                    if e == panel:

                                        key = group['title']
                                        if key not in tmpData:
                                            tmpData[key] = { }

                                        tmpData[key][panel] = panelValue

                                        # add key to the list if not yet there
                                        if key not in oscData.keys():
                                            oscData[key] = ([], [])

                            # print(f'handled column {collumn}')
                            collumn += 1

                        # add data only if there is something
                        if key != None:
                            avg = sum(tmpData[key].values()) / len(tmpData[key])

                            # print(f'{now} - {key}: {avg}')
                            # print(tmpData[key])

                            oscData[key][0].append(now)
                            oscData[key][1].append(avg)

                            # make sure we have a upper limit
                            if panel in oscData.keys() and len(oscData[panel][0]) > maxSamplesPerLine:
                                oscData[key][0].pop(0)
                                oscData[key][1].pop(0)

        finally:
            plotDataLock.release()

        # update should exit flag (thread safe)
        oscDecoderThrdRunningLock.acquire()
        try:
            thrdRunning = oscDecoderThrdRunning
        finally:
            oscDecoderThrdRunningLock.release()

    prints('Read and decode thread stopped', style.OK)

#--------------------------------
def receiverTask(port, oscAddrFilter):

    prints("Starting OSC receiver", style.BLUE)

    # initialize SLIP serial
    slip = SlipSerial()
    if not slip.open(port):
        exit(-1)

    # run main loop
    try:
        while True:

            # read data and decode osc messages
            packetList = slip.read()
            msgs = decodeOsc(packetList)

            # print messages (filtered)
            for msg in msgs:

                # filter osc messages
                if type(oscAddrFilter) != str:

                    found = False
                    for addr in oscAddrFilter:
                        if addr in msg['address']:
                            found = True
                            break

                    if not found:
                        continue

                elif oscAddrFilter not in msg['address']:
                    continue

                # check for valid data
                if len(msg['values']) == 0:
                    continue

                print(msg)

    except KeyboardInterrupt:
        pass

    finally:
        slip.close()

    prints("OSC receiver stopped", style.BLUE)

#--------------------------------
if __name__ == '__main__':

    import sys
    
    if len(sys.argv) < 3:
        prints('Invalid arguments! arg1 = serial port, arg2 = oscAddress', style.ERR)
        exit(-1)

    addressArg = sys.argv[2]
    if len(addressArg.split(',')) > 1:
        oscAddrFilter = addressArg.split(',')
    else:
        oscAddrFilter = addressArg

    prints(f'Filtering OSC address: {oscAddrFilter}')

    # argument 1 ... serial port
    # argument 2 ... OSC address filter (defaults to '/raw/')
    receiverTask(sys.argv[1], oscAddrFilter)
    prints('Have a nice day', style.BLUE)