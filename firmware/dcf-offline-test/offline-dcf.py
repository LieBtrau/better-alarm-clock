# https://www.dcf77logs.de/logs
# https://www.dcf77logs.de/ereignisse
# https://medium.com/@eleroy/10-things-you-need-to-know-about-date-and-time-in-python-with-datetime-pytz-dateutil-timedelta-309bfbafb3f7

import serial
import datetime
import pytz
import dateutil.parser

# dtString = 'So, 11.11.18 23:56:00, WZ'
# So, 30.10.11 02:59:00, SZ
# So, 30.10.11 02:00:00, WZ
# So, 27.03.11 01:59:00, WZ
# So, 27.03.11 03:00:00, SZ
def datestring2unixepoch(dtString):
    "Convert a date in a string, e.g. 'So, 27.03.11 03:00:00, SZ' to unix epoch time."
    dtString = dtString[dtString.find(' '):].strip()
    dtString = dtString.replace('WZ', 'CET')
    dtString = dtString.replace('SZ', 'CEST')
    d = dateutil.parser.parse(dtString)
    utc = pytz.UTC
    d = d.astimezone(utc)
    epoch = datetime.datetime(1970,1,1,0,0,0,tzinfo=pytz.UTC)
    ts = (d - epoch).total_seconds() 
    return ts

def readFile(filename):
    "This function reads a file line by line and returns a list of the lines"
    with open(filename) as f:
        lines = f.readlines()
    lines = [x.strip() for x in lines]
    return lines

def parseLines(lines):
    "Convert file lines to a list of tuples containing the dcf-signal as decimal value and the corresponding unix epoch"
    retList=[]
    for line in lines:
        if line.startswith('0 '):
            endDataPos = line.find('  ')
            if endDataPos != -1:
                databits = line[:endDataPos].replace(' ', '')[::-1]
                if databits.isdigit():
                    databits = hex(int(databits, 2))
                    datestring = line[endDataPos:].strip()
                    epoch = datestring2unixepoch(datestring)
                    tup = (databits, epoch)
                    retList.append(tup)
    return retList

fileLines = readFile('DcfLog_20181111.log')
pData = parseLines(fileLines)
print pData

# ser = serial.Serial(
#     port='/dev/ttyUSB0',
#     baudrate=115200,
#     bytesize=serial.EIGHTBITS,
#     parity=serial.PARITY_NONE,
#     stopbits=serial.STOPBITS_ONE
# )
# ser.isOpen()

# ser.close()