this library depends on 3 modules and 2 dlls in order to work.

boost provides boost_python which is a c++ template-interface to python. this is
a giant library, and it's located at http://www.boost.org. one can find an
installer at http://www.boostpro.com/download

$ bjam.exe variant=release link=static,shared threading=multi embed-manifest=on --architecture=x86 --address-model=32,64
$ bjam.exe variant=release link=shared threading=multi embed-manifest=on --architecture=x86 --address-model=32,64

python provides the ability to utilize the python language. this can be fetched
from http://www.python.org. the latest version tested is 2.6

windbg will allow one to use the debugging api. unfortunately this library has only
been developed/tested on WoW64. the x86 version of the debugging tools can be downloaded
direct from microsoft at http://www.microsoft.com/whdc/devtools/debugging/installx86.mspx
this will also contain the debugging engine api dlls.

once getting the prerequisites, one will need the static libraries and the header files
to link and compile with. in each of the installed directories

boost/
    boost - at $INSTALLDIR/boost
    stage - at $INSTALLDIR/stage

python/
    include - located at $INSTALLDIR/include
    libs - located at $INSTALLDIR/libs

windbg/
    inc - found at $INSTALLDIR/inc
    lib - found at $INSTALLDIR/lib
    bin/dbgeng.dll - found at $INSTALLDIR/$arch/dbgeng.dll
    bin/dbghelp.dll - found at $INSTALLDIR/$arch/dbghelp.dll
	bin/kldbgdrv.sys - found as a resource 17476/30583 embedded within $INSTALLDIR/windbg.exe

This will allow one to build the project with full functionality.

By the way, when building a Debug Release, "sometimes" Microsoft's linker will crash. But,
just keep hitting rebuild anyways, because sometimes it doesn't crash. ;)

Thanks for reading!
