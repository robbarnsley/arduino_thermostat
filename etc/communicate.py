import time

class to_arduino():
  def __init__(self, ser):
    self.ser = ser

  def write_serial(self, cmd):
    self.ser.flushInput()
    self.ser.flushOutput()
    if not cmd.endswith('\n'):
      cmd = cmd + '\n'
    self.ser.write(cmd)
    self.ser.flush()
    return

  def send_command(self, cmd):
    while True:
      self.write_serial(cmd)
      reply = self.listen_serial()
      if reply != None:
        return reply

  def listen_serial(self, timeout=2):
    msg = ""
    now = time.time()
    while(time.time()-now <= timeout):
      while self.ser.inWaiting() > 0:
        value = self.ser.read()
        msg = msg+value
        if value == '\n':
          return msg

