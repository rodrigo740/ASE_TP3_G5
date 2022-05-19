#Writes/reads to & from 25LC512
#writes 0x33 to address 0x0001 and reads back

import spidev
import time
spi=spidev.SpiDev()
spi.open(0, 0)
spi.max_speed_hz=int(8000)

write_en=0x06
write_cmd=0x02
read_cmd=0x03
Hi_Addr=0x00
Lo_Addr=0x01
data0=0xDD
dummy=0x00

spi.writebytes([write_en])

write_sequence0=[write_cmd, Lo_Addr, data0]
read_sequence0=[read_cmd, Lo_Addr]

#write 0xF33 to address 0x0001
spi.xfer3(write_sequence0)

time.sleep(0.005)  #delay 5ms
#read 1 byte from 0x0001
read_data=spi.xfer3(read_sequence0)
a = spi.readbytes(512)
for i in a:
    if i != 0:
        print(i)
print(a)
print(read_data)