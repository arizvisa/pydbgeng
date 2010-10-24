import ptypes
import PyDbgEng
class debugclientprovider(object):
    def __init__(self, client=None):
        if client is None:
            client = PyDbgEng.DebugClient.Current
        self.dataspace = client.DataSpaces.Virtual
        self.address = 0

    def seek(self, address):
        self.address = address

    def consume(self, amount):
        res = self.dataspace.Read(self.address, amount)
        self.address += amount
        return str(res)

    def write(self, value):
        self.address += len(value)
        res = self.dataspace.Write(self.address, str(value))
        return res
