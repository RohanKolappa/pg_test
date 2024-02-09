
Extract v2drxlib_xxxxx.zip (Windows version)or v2drxlib_xxxxx.tar (Linux version) into any folder.
It contains bin, lib, include, and src folders.
  -- bin folder contains rx binary (and v2drxlib dynamic load library in-case of Windows).
  -- lib folder contains v2drxlib library file.
  -- include folder contains all the necessary header files.
  -- src folder contains rx.cpp, and pgetopt.c files.

rx.cpp is a sample program to demonstrate "how to use v2drx library".


How to use rx binary:

rx binary takes at least one parameter i.e. host address -s.
  --in Windows
   rx.exe -s <TX_IPADDRESS>

  --in Linux
    To run rx binary in linux, "/lib" folder path has to be added to LD_LIBRARY_PATH.

   rx -s <TX_IPADDRESS>

For detailed usage run "rx -h".

Creating raw bitmap data:

rx -s 10.51.50.17 -B 1

It will generate "BitmapData.dat" file in the current directory.

Creating rx binary in Windows:
  -- Create a "Win32 Console Application" project in Visual Studio
  -- Add "src/rx.cpp and src/pgetopt.c to source files
  -- Add "include" folder to "Additional Include Directories"
  -- Add "lib/v2drxlib.lib" to "Additional Dependencies" list.
  -- Add "ws2_32" to "Additional Dependencies" list

Creating rx binary in Linux (Tested in Fedora, and Debian):
  -- cd to src folder
  -- run "g++ -I ../include -L ../lib -lv2drx -o rx  rx.cpp pgetopt.c"


V2drx library API document (V2DRxLibAPI.pdf) is provided separately.