== Intro ==
This is some modifications based on flierlu's PyDbgEng code. I was having issues with
races occurring, issues with strange crashes, etc. So I ended up hacking some code
into it. Building this is kind of sketchy, but after installation of each of the
components of the Modules/ directory, one will just need to go to the Binary/Release
location and copy the following files out. Simply deploying these is all that's
required to use it.

PyDbgEng.pyd
PyDbgExt.dll
dbgeng.dll
dbghelp.dll
boost_python-*.dll

== Building ==
See Readme-Building.txt

== Directory Summary ==
Document ->	documentation
Source -> 	source code
Modules ->	modules that need .lib and header files from the builder also dbgeng binaries
Output ->	output for all .obj and other intermediary files
Binary ->	target executables

== PyDbgEng ==
This provides the interface to the windbg engine. It depends on dbgeng.dll being
available. For building this will depend on the binaries located in Modules/windbg.
As far as I can tell, there's no effort in this module to pretty up the dbgeng
IDebugClient api. So if you want to add a function that doesn't exist, try and
keep it as minimal as possible. Any features should be handled in the python module
that wraps it.

== PyDbgExt ==
This module will provide python support within your debugger's commandline.

To install this, copy PyDbgExt.dll along with PyDbgEng.pyd to windbg's ./winext
directory. This should cause it to get loaded on startup. If it's not loaded, you
can use:

0:000> .load pydbgext

On load, the winext/ directory will get added to the default python search path,
and then import PyDbgEng. On completion of initialization of the plugin, the
$env:USERPROFILE/pydbgextrc.py script will be executed. This is intended for allowing
a user to specify a global namespace for the extension.

One a debugger instance has been activated, the $env:USERPROFILE/pydbgextsess.py
will be executed, this will allow a user to populate a namespace local to the
session they are currently in.

== Scripts ==
Example rc files for using with the extension.
