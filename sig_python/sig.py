# Reading *.sig (and probably *.sigf) files recorded
# by pico_osc program (https://github.com/slazav/pico_osc)

import numpy
import re

def read(name):
  fo=open(name,'rb')

  info={
    "dt":     0.0,
    "t0":     0.0,
    "t0abs":  0.0,
    "points": 0,
    "navr":   0,
    "ch":  [],  # channel names (A,B,C...)
    "sc":  [],  # scales for each channels (integer data to voltage conversion factor)
    "ov":  [],  # overload flags for each channel
    # over field will be written as strings
  }
  info["chans"]=[]
  info["chans"]=[]
  while 1:
    line = fo.readline().decode('ascii')
    if not line:
      print("no data found!")
      exit(1)

    line = re.sub(r'#.*$','',line)
    m = re.match(r'^\s+(\S+):\s*(.*)$', line)
    if m:
      k = m.group(1)
      v = m.group(2)
      if k in ("dt", "t0", "t0abs"):
        info[k]=float(v)
      elif k in ("points", "navr"):
        info[k]=int(v)
      elif k=="chan":
        (n,sc,ov) = re.split(r'\s+', v)
        info["ch"].append(n)
        info["sc"].append(float(sc))
        info["ov"].append(bool(ov))
      else:
        info[k]=v

    if re.fullmatch(r'^\*\s*$',line):
      break
  nch = len(info["ch"])
  data = numpy.fromfile(fo, dtype='int16')
  data = numpy.reshape(data, (-1,nch))
  data = numpy.transpose(data)
  data = data.astype(float)
  for i in range(nch):
    data[i,:] *= info["sc"][i]
  return (data, info)

def make_tgrid(info):
  npt = info["points"]
  dt  = info["dt"]
  time=numpy.linspace(0,dt*(npt-1),npt)
  return time

