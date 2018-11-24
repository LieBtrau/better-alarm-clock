# https://www.dcf77logs.de/logs
# https://www.dcf77logs.de/ereignisse
# https://medium.com/@eleroy/10-things-you-need-to-know-about-date-and-time-in-python-with-datetime-pytz-dateutil-timedelta-309bfbafb3f7

import serial
import datetime
import pytz
import dateutil.parser


def datestring2unixepoch(dtString):
    "Convert a date in a string, e.g. 'So, 27.03.11 03:00:00, SZ' to unix epoch time."
    # Splitting and parsing string ourselves because dateutil.parse swaps months and days and strptime doesn't know how to handle time zones.
    data = dtString.split()
    date = data[1].split('.')
    time = data[2].split(':')
    if data[3]=='SZ':
        tOffset='+02:00'
    elif data[3]=='WZ':
        tOffset='+01:00'
    iso8601time =  str(int(date[2])+2000)+'-'+ date[1] +'-'+ date[0]+'T'+time[0]+':'+time[1]+':00'+tOffset
    d = dateutil.parser.parse(iso8601time)
    d = d.replace(tzinfo=pytz.UTC) - d.utcoffset()
    epoch = datetime.datetime(1970, 1, 1, 0, 0, 0, tzinfo=pytz.UTC)
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
    retList = []
    for line in lines:
        if line.startswith('0 '):
            endDataPos = line.find('  ')
            if endDataPos != -1:
                databits = line[:endDataPos].replace(' ', '')[::-1]
                if databits.isdigit():
                    databits = int(databits, 2)
                    datestring = line[endDataPos:].strip()
                    epoch = datestring2unixepoch(datestring)
                    tup = (databits, epoch, datestring)
                    retList.append(tup)
    return retList


fileLines = readFile('Winterzeit.log')
# fileLines = readFile('Sommerzeit.log')
# fileLines = readFile('Jahreswechsel.log')
# fileLines = readFile('DcfLog_20181108.log')
pData = parseLines(fileLines)

ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=115200,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE
)
ser.isOpen

print "\nMake sure to reset the BluePill before running this test."
succes = 0
for tup in pData:
    ser.write(hex(tup[0])+'Z')
    readEpoch = int(ser.readline())
    if readEpoch == tup[1]:
        succes = succes+1
    else:
        print "Error: Soll: " + str(tup[1]) + "\tIst: " + str(
            readEpoch) + "\tDifference : " + str(readEpoch - tup[1]) + '\t' + tup[2] + '\t' + str(tup[0])
    # print str(tup[0])+'\t'+str(tup[1])+'\t'+str(tup[2])
    # print str(tup[0])+','
print str(succes) + ' minutes ok of ' + str(len(pData)) + ' minutes'
ser.close()
