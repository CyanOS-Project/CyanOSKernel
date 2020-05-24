# CyanOS: A Simple Hobbyist Operating System

## Building CyanOS
### Windows
#### Setting up
* The kernel is compiled using `g++` so you need to download the latest version of [MinGW32](https://osdn.net/projects/mingw/releases/).
* From `MinGW Installation Manager`, make sure you already have installed `mingw-binutils`, `mingw-gcc`, `mingw-gcc-g++`, `mingw-gdb` and `mingw-make`.
* Add the `bin` folder path of `mingw` to the environment variables.
* For emulating and testing the operating system you need to download 32bit version of [qemu](https://www.qemu.org/download/).
* Add qemu main folder's path to the environment variables.
#### Building
Using `git` or `Cygwin` bash, enter the main source code directory and type one of the following commands
```
make        // To compile
make run    // To compile and run the OS directly in qemu.
make debug  // To compile and start a debugging session.
```

### Linux (Ubuntu)
#### Setting up
Using bash type the following commands
```
sudo apt-get update
sudo apt-get install build-essential gcc-multilib g++-multilib qemu
```
#### Building
Using bash, enter the main source code directory and type one of the following commands
```
make        // To compile
make run    // To compile and run the OS directly in qemu.
make debug  // To compile and start a debugging session.
```
You may need to change the name of qemu executable in /kernel/Makefile under the variable `QEMU` to your qemu name.