this library depends on 3 modules in order to work.

boost provides boost_python which is a c++ template-interface to python. this is
a giant library, and it's located at http://www.boost.org. one can find an
installer at http://www.boostpro.com/download

python provides the ability to utilize the python language. this can be fetched
from http://www.python.org. the latest version tested is 2.6

windbg will allow one to use the debugging api. unfortunately this library has only
been developed/tested on WoW64. the x86 version of the debugging tools can be downloaded
direct from microsoft at http://www.microsoft.com/whdc/devtools/debugging/installx86.mspx

once getting the prerequisites, one will need the static libraries and the header files
to link and compile with. in each of the installed directories 

boost/
    include/boost - at $INSTALLDIR/$boost_ver/boost
    libs - at $INSTALLDIR/$boost_ver/lib

python/
    include - located at $INSTALLDIR/include
    libs - located at $INSTALLDIR/libs

windbg/
    inc - found at $INSTALLDIR/sdk/inc
    lib - found at $INSTALLDIR/sdk/lib

This will allow one to build the project.

Thanks for reading!
