import serial
import time
import sys
from communicate import to_arduino

if __name__ == "__main__":
  ser = serial.Serial('/dev/ttyACM0', 9600)
  talk = to_arduino(ser)
  talk.send_command("NEXTSTAT?") # some reason first command always fails..
  talk.write_serial("SETSETPO?" + sys.argv[1])
  ser.close()
