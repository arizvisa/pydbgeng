import sys
sys.path.append('f:/work/pydbgext')
sys.path.append('f:/work/pydbgext/Binary/Release')

import PyDbgEng
from PyDbgEng import *

TINYUNITOFTIME=1
class IDebugClient(object):
    def __init__(self, client=None):
        if client is None:
            client = PyDbgEng.DebugClient()
        self.c = client
        self.__breakpoints = {}

    ## creation/destruction
    def open(self, executable, args):
        raise NotImplementedError

    def attach(self, processid):
#        raise NotImplementedError("""
#        It seems like AttachProcess doesn't seem to set g_process which is a
#        global. I found this out in dbgeng.dll. There must be something else
#        that has to be done. Probably with IDebugSystemObjects?
#        http://msdn.microsoft.com/en-us/library/ff550875(v=VS.85).aspx
#        """)
#        res = self.c.AttachProcess(processid, flags=AttachUserFlags.INVASIVE_NO_INITIAL_BREAK)
        res = self.c.AttachProcess(processid)     #, flags=AttachUserFlags.INVASIVE_NO_INITIAL_BREAK)

#        raise NotImplementedError("""
#        WaitForEvent will set g_process, but for some reason in my testing it's not...what the fuck...
#        """)
        e = self.c.Control.WaitForEvent(TINYUNITOFTIME)  # XXX: check to make sure e == S_FALSE
        if e != 0:
            raise OSError( e )
        return res

    def detach(self):
        return self.c.DetachProcesses()

    def close(self):
        return self.c.TerminateProcesses()

    ## process manipulation
    def go(self, timeout=1):
        return self.c.Control.WaitForEvent(timeout)

    ## callbacks
    def __createprocess(self, args):
        print "CreateProc: %08x-%08x %s\t%s" % (args.BaseOffset, args.BaseOffset+args.ModuleSize, args.ModuleName, args.ImageName)

    def __exitprocess(self, args):
        print "ExitProcess %d" % args.ExitCode

    def __createthread(self, args):
        print "CreateThread %x %08x %08x" % (args.Handle, args.DataOffset, args.StartOffset)

    def __exitthread(self, args):
        print "ExitThread %d" % args.ExitCode

    def __loadmodule(self, args):
        print "ModLoad: %08x-%08x %s\t%s" % ( args.BaseOffset, args.BaseOffset+args.ModuleSize, args.ModuleName, args.ImageName)

    def __breakpoint(self, args):
        bp = args.Breakpoint
        id = bp.Id
        print "Hit Breakpoint: %s %d @ %08x %s:%d" % (str(bp.Type[0]), bp.Id, bp.Offset)

        try:
            fn = self.__breakpoints[id]
        except KeyError:
            return ExecutionStatus.BREAK
        return fn(args)

    def __exception(self, args):
        symbol = c.Symbols.GetNameByOffset(args.Address)
        res = ["first", "second"][not bool(args.IsFirstChance)]
        print "Exception: %08x %08x %s" % (args.Code, args.Address, res)

    def setcallbacks(self):
        c = self.c
        c.EventCallbacks.CreateProcess = lambda args: self.__createprocess(args)
        c.EventCallbacks.ExitProcess = lambda args: self.__exitprocess(args)
        c.EventCallbacks.CreateThread = lambda args: self.__createthread(args)
        c.EventCallbacks.ExitThread = lambda args: self.__exitthread(args)
        c.EventCallbacks.LoadModule = lambda args: self.__loadmodule(args)
        c.EventCallbacks.Breakpoint = lambda args: self.__breakpoint(args)
        c.EventCallbacks.Exception = lambda args: self.__exception(args)
        c.EventCallbacks.Attach()

    def bpx(self, address, fn):
        c = self.c
        bp = c.Control.AddBreakpoint()
        id = address

        print 'offset'
        bp.offset = address

        if id in self.__breakpoints:
            print "Replace Breakpoint: %s %d @ %08x" % (str(bp.Type[0]), bp.Id, bp.Offset)
        else:
            print "Add Breakpoint: %s %d @ %08x" % (str(bp.Type[0]), bp.Id, bp.Offset)

        self.__breakpoints[id] = fn
        return id

    def unbreak(self, id):
        raise NotImplementedError

if __name__ == '__main__':
    import test; reload(test)
    module=test

    address = 0x76c61126 

    pid = int(sys.argv[1])
#    pid = 1868
    
    self = module.IDebugClient()
    self.setcallbacks()

    self.attach(pid)

    def displayshit(*args, **kwds):
        print 'ok'

    self.bpx(address, displayshit)
    print self.go(100000)    # go for 10000 ms

    print self.detach()
