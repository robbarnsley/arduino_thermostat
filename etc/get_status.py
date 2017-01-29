import serial
import time

from communicate import to_arduino

DELAY = 60 

if __name__ == "__main__":
  ser = serial.Serial('/dev/ttyACM0', 9600)
  talk = to_arduino(ser)
  talk.send_command("NEXTSTAT?") # some reason first command always fails..
  t = []
  T_0 = []
  T_1 = []
  T_2 = []
  T_3 = []
  while True:
    now = time.time()
    t.append(now)
    NEXTSTAT = talk.send_command("NEXTSTAT?").rstrip('\r\n')
    ISHEATON = talk.send_command("ISHEATON?").rstrip('\r\n')
    GETSETPO = talk.send_command("GETSETPO?").rstrip('\r\n')
    THIS_T_0 = talk.send_command("GETTEMPC?0").rstrip('\r\n')
    THIS_T_1 = talk.send_command("GETTEMPC?1").rstrip('\r\n')
    THIS_T_2 = talk.send_command("GETTEMPC?2").rstrip('\r\n')
    THIS_T_3 = talk.send_command("GETTEMPC?3").rstrip('\r\n')
    with open("data", 'a') as file:
      file.write(str(now) + '\t' + str(THIS_T_0) + '\t' + str(THIS_T_1) + '\t' +
                 str(THIS_T_2) + '\t' + str(THIS_T_3) + '\t' + str(GETSETPO) + 
                 '\t' + str(ISHEATON) + '\t' + str(NEXTSTAT) + '\n')

    print str(now) + '\t' + str(THIS_T_0) + '\t' + str(THIS_T_1) + '\t' + \
                 str(THIS_T_2) + '\t' + str(THIS_T_3) + '\t' + str(GETSETPO) + \
                 '\t' + str(ISHEATON) + '\t' + str(NEXTSTAT)
 
    T_0.append(THIS_T_0)
    T_1.append(THIS_T_1)
    T_2.append(THIS_T_2)
    T_3.append(THIS_T_3)

    time.sleep(DELAY)
  ser.close()

