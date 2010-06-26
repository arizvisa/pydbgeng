def dumpRegister(reg):
  import sys, PyDbgEng

  v = reg.Value

  if reg.Type == PyDbgEng.DebugRegisters.RegisterType.INT32:
    return hex(ord(v[3]) * 0x1000000 + ord(v[2]) * 0x10000 + ord(v[1]) * 0x100 + ord(v[0]))

  r = ""

  for i in range(len(v)):
    r += "%d, " % ord(v[i])

  return r

def reg():
  import PyDbgEng

  dc = PyDbgEng.DebugClient.Current
  regs = dc.Registers

  for r in regs.Registers:
    msg = "[%02d] %s : %s = [" % (r.Index, r.Name, r.Type)
    msg += dumpRegister(r)
    msg += "]"

    print msg