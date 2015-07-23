
VELaSSCo Access Library
=======================

Building:
---------

To build the access library, you need the following:

 - Visual Studio 2012 Win 64 (no other platforms tested)
 - CMake >= 2.8.11

Build process:

 - Unzip "3rdParty.zip" in the current folder.
 - Open CMake.
 - As source path enter the current directory "<path>/modules/AccessLib".
 - As build path enter "<path>/modules/AccessLib/build".
 - Press "Configure" and select "Visual Studio 11 Win64".
 - Press "Configure" until no lines are marked red.
 - Press "Generate".
 - Open "<path>/modules/AccessLib/build/VELaSSCo_AccessLib.sln".
 - Compile in Visual Studio.

Testing:
-------

The Visual Studio solution also contains projects for testing
("Test_Client" and "Test_Server"). Simply, first start
"Test_Sever.exe". This opens a VELaSSCo server using port 9090. Then
start "Test_Client.exe". The client test executable logs in, issues a
query and logs out.
