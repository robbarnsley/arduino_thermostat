import time
import datetime

import matplotlib
import matplotlib.dates as md
matplotlib.use("cairo")
import matplotlib.pyplot as plt

DELAY = 60

while True:
  t = []
  T_0 = []
  T_1 = []
  T_2 = []
  T_3 = []

  with open("data", 'r') as file:
    for line in file:
      try:
        t.append(datetime.datetime.fromtimestamp(int(round(float(line.split()[0])))))
        T_0.append(line.split()[1])
        T_1.append(line.split()[2])
        T_2.append(line.split()[3])
        T_3.append(line.split()[4])
        DMD = line.split()[5]
      except:
        pass

  plt.plot(t, T_0, label="T(0)")
  plt.plot(t, T_1, label="T(1)")
  plt.plot(t, T_2, label="T(2)")
  plt.plot(t, T_3, label="T(3)")
  plt.axhline(y=DMD, label="Demand")
  plt.legend()
  ax = plt.gca()
  xfmt = md.DateFormatter('%Y-%m-%d %H:%M')
  ax.xaxis.set_major_formatter(xfmt)
  plt.savefig("status.png")
  plt.clf()
  time.sleep(DELAY)
