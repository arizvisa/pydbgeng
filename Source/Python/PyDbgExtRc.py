# This is the default rc file that runs when windbg starts.
# this also executes it on every windbg session
# Your debugger.client should be guaranteed at this point.

import os,sys
sys.path.append(os.environ['USERPROFILE'])

# windbg-masm syntax
def poi(address):
    import ptypes
    return int(ptypes.pint.uint32_t(offset=address).l)

class remote(object):
	'''Create an object used for converting addresses between two different bases'''
    def __init__(self, staticbase, loadedbase):
        if loadedbase is None:
            # XXX: figure out the base address of some module
            loadedbase = 0

        self.rbase = staticbase
        self.lbase = loadedbase

    def get(self, address):
        return address - self.rbase + self.lbase

    def put(self, address):
        return address - self.lbase + self.rbase

print 'ok'
