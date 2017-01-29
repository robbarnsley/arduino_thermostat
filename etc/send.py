import serial
import time

DELAY = 1 

def listen_serial(serial, timeout=2):
  msg = ""
  now = time.time()
  while(time.time()-now <= timeout):
    while serial.inWaiting() > 0:
      value = serial.read()
      msg = msg+value
      if value == '\n':
        return msg
            
def write_serial(serial, cmd):
  ser.flushInput()
  ser.flushOutput()
  if not cmd.endswith('\n'):
    cmd = cmd + '\n'
  serial.write(cmd)
  serial.flush()
  return

def send_command(serial, cmd):
  while True:
    write_serial(serial, cmd)
    reply = listen_serial(serial)
    if reply != None:
      return reply

if __name__ == "__main__":
  ser = serial.Serial('/dev/ttyACM0', 9600)
  send_command(ser, "NEXTSTAT?") # some reason first command always fails..

  t = []
  T_0 = []
  T_1 = []
  T_2 = []
  T_3 = []
  while True:
    now = time.time()

    t.append(now)
    NEXTSTAT = send_command(ser, "NEXTSTAT?").rstrip('\r\n')
    ISHEATON = send_command(ser, "ISHEATON?").rstrip('\r\n')
    GETSETPO = send_command(ser, "GETSETPO?").rstrip('\r\n')
    THIS_T_0 = send_command(ser, "GETTEMPC?0").rstrip('\r\n')
    THIS_T_1 = send_command(ser, "GETTEMPC?1").rstrip('\r\n')
    THIS_T_2 = send_command(ser, "GETTEMPC?2").rstrip('\r\n')
    THIS_T_3 = send_command(ser, "GETTEMPC?3").rstrip('\r\n')

    with open("data", 'a') as file:
      file.write(str(now) + '\t' + str(THIS_T_0) + '\t' + str(THIS_T_1) + '\t' +
                 str(THIS_T_2) + '\t' + str(THIS_T_3) + '\t' + str(GETSETPO) + 
                 '\t' + str(ISHEATON) + '\t' + str(NEXTSTAT) + '\n')

    T_0.append(THIS_T_0)
    T_1.append(THIS_T_1)
    T_2.append(THIS_T_2)
    T_3.append(THIS_T_3)

    time.sleep(DELAY)
 
  ser.close()

