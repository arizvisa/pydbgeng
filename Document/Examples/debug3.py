import sys
import PyDbgEng

a = PyDbgEng.Create()
print a.AttachKernel()

z = PyDbgEng.Connect('tcp:port=57005,server=127.0.0.1')
z = PyDbgEng.ConnectKernel()
z = PyDbgEng.ConnectKernel('')
z = PyDbgEng.ConnectKernel(r'com:pipe,port=\\.\pipe\com_1,resets=0')
