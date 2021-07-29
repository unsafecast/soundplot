# soundplot

![Screenshot of soundplot](/screenshot.png)
_soundplot displaying part of "the duck song", the black region is a bug_

This is a simple program that visualizes your sound files. It accepts a raw sound file and it creates a window, the plots the hilbert curve for every sample, with the color value being the frequency of it.

This project was inspired by [this](https://www.youtube.com/watch?v=3s7h2MHQtxc&t=147s) video by 3Blue1Brown.

### Building
The build system doesn't require anything other than having a C compiler.

To bootstrap the build system, run `cc -o nobuild nobuild.c`. This will create the build system executable, which you can run to build the project itself.

This currently only works on Linux with raylib installed globally on the system. Windows/MacOS support is trivial to add, it's just a matter of adapting the build system.

