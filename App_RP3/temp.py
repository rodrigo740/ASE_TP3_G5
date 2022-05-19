import smbus
from time import sleep

def readTemp():
    channel = 1
    address = 0x4d
    bus = smbus.SMBus(channel)
    data = bus.read_byte_data(address, 0)
    print(data)
    #sleep(1)
    return data