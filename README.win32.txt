/* vim: set ts=4 sw=4 tw=72: */

This project will compile on win32 with MinGW, here's some info on how to do this.

1. Install and prepare MinGW

The easies way is to download Automated MinGW Installer
(http://sourceforge.net/projects/mingw/files/Automated%20MinGW%20Installer/mingw-get-inst-20101030/mingw-get-inst-20101030.exe/download)
Use pre-packaged lists, install to somewhere without spaces, e.g.
C:\MinGW. On the screen with installer options add these two:
* C++ Compiler
* MinGW Developer ToolKit

In my case antivirus software started to scream about some trojan or
else. If that happens to you, try adding an exception C:\MinGW directory
or use your favourite way to deal with it. Well, on the other hand I
can't guarantee that there's nothing harmless.

Now add MinGW /bin directory to your PATH enviroment variable (Control
Panel -> System -> Advanced -> Environment variables).

Start the commandline, then update and install some packages:
cmd> mingw-get update
cmd> mingw-get install msys-w32api
cmd> mingw-get install mingw32-gcc
cmd> mingw-get install mingw32-make

Now you can close the commandline.

2. Install Qt for Windows/MinGW

We've had successful build with:
Qt libraries 4.7.1 for Windows (minGW 4.4, 319 MB)
http://get.qt.nokia.com/qt/source/qt-win-opensource-4.7.1-mingw.exe

Installing to default location (C:\Qt\4.7.1) is OK. Once again, you'd
rather install in space-less path.

3. Download and install freeglut for MinGW

Project depends on GLUT extensions to OpenGL, but on Windows it's better
to use freeglut implementation. Download it from:
http://www.martinpayne.me.uk/software/development/GLUT/freeglut-MinGW.zip

After extracting, follow installation guide (copy freeglut/include to
C:\MinGW\include and freeglut/lib to C:\MinGW\lib)

4. Download and install Gnu Scientific Library (GSL)

Get it from
http://sourceforge.net/projects/gnuwin32/files/gsl/1.8/gsl-1.8.exe/download

I recomment installing it in space-less location, e.g. c:\GnuWin32
(note: the default is Program Files\GnuWin32 -- if you don't know which
to choose, choose our option: C:\GnuWin32)

5. Compile project

Provided that you followed our advices about certain libraries
locations, you can now compile the project using provided Makefile.win.

Start MSYS (it installed with MinGW, check your start menu, programs,
MinGW). Once in MSYS shell, navigate to where you downloaded our
project, e.g /c/graph. Then invoke mingw make on Makefile.win:
msys> mingw32-make -f Makefile.win

It should compile OK and produce graph.exe in dist directory.

6. Running

In order to run this project, you need 3 dlls:
* freeglut.dll (from freeglut zip)
* libgslcblas.dll (from GSL zip)
* libgsl.dll (from GSL zip)

Either place them in the same directory where graph.exe is located, or
in system visible location (e.g. %%WINDIR%%\system32). Of course, the
program requires some Qt dlls, so if you'd like to share the exe with
somebody else, provide them too.

7. I wan't do to it my way.

Well, if you installed libs to somewhere else or don't want to use
MinGW, you should change the Makefile.win or even develop your own
instruction how to do this. General guidelines (Qt, freeglut and GSL)
still apply.


