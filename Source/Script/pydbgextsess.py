import PyDbgEng

client = PyDbgEng.DebugClient.Current
dataspace = client.DataSpaces.Virtual
registers = client.Registers.Registers