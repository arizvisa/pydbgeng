this library depends on 3 sdks and 2 external dlls in order to work.

boost provides boost_python which is a c++ template-interface to python. this is
a giant template library, and it's located at http://www.boost.org. one can find an
installer at http://www.boostpro.com/download. if it is necessary to build boost
from source, make sure to enable python support and build shared multithreaded dll's.

    toolset={msvc-12.0, msvc-11.0}
	./bjam.exe --address-model=32,64 --with-python --toolset=$toolset
	./b2 link=shared,static threading=single,multi --with-python --toolset=$toolset

python provides the ability to utilize the python language. this can be fetched
from http://www.python.org. the latest version tested is 2.7

windbg will allow one to use the debugging api. unfortunately this library has only
been developed/tested on WoW64. the x86 version of the debugging tools can be downloaded
direct from microsoft at http://www.microsoft.com/whdc/devtools/debugging/installx86.mspx
this will also contain the debugging engine api dlls.

once getting the prerequisites, one will need to stash the static libs and header files
in the following directory structure:

boost/
    include/boost - at $INSTALLDIR/$boost_ver/boost
    lib - at $INSTALLDIR/$boost_ver/lib

python/
    include - located at $INSTALLDIR/include
    libs - located at $INSTALLDIR/libs

windbg/
    inc - found at $INSTALLDIR/sdk/inc
    lib - found at $INSTALLDIR/sdk/lib
    dbgeng.dll - found at $INSTALLDIR/dbgeng.dll
    dbghelp.dll - found at $INSTALLDIR/dbghelp.dll
	kldbgdrv.sys - found as a resource 17476/30583 embedded within $INSTALLDIR/windbg.exe

This will allow one to build the project with full functionality. This has been
tested with Visual Studio 2010, 2012, and 2013. To build via msbuild.exe, 
    
    plat={x86, x64}
    config={Release, Debug}
    toolset={v110, v120}
    ./msbuild.exe /p:Configuration=$config /p:Platform=$plat /p:PlatformToolset=$toolset

By the way, when building a Debug Release, "sometimes" Microsoft's linker will crash. But,
just keep hitting rebuild anyways, because sometimes it doesn't crash. ;)

Thanks for reading!
