# CyanOS: A Hobbyist Operating System

## Todo list
- [x] Paging and virtual memory.
- [x] Higher half kernel
- [x] Interrupt dispatchers.
- [x] Dynamic virtual/physical memory allocation.
- [ ] Heap allocator.
- [ ] PIC.
- [ ] Multiprocessing.
- [ ] Mutex/Semaphores.
- [ ] User space.
- [ ] System calls.
- [ ] Keyboard/Mouse drivers.
- [ ] Process signalling.
- [ ] Filesystem.<br><br>
Distance Future:<br>
- [ ] GUI.
- [ ] Network Driver.
- [ ] USB.

## Building CyanOS
### Windows
#### Setting up
* The kernel is compiled using `g++` so you need to download the latest version of [MinGW32](https://osdn.net/projects/mingw/releases/).
* From `MinGW Installation Manager`, make sure you already have installed `mingw-binutils`, `mingw-gcc`, `mingw-gcc-g++`, `mingw-gdb` and `mingw-make`.
* Add the `bin` folder path of `mingw` to the environment variables.
* Download [nasm](https://www.nasm.us/) and add it's folder to the environment variables.
* For emulating and testing the operating system you need to download 32bit version of [qemu](https://www.qemu.org/download/).
* Add qemu main folder's path to the environment variables.
#### Building
Using `git` or `Cygwin` bash, type the following commands
```
git clone https://github.com/AymenSekhri/CyanOS.git
cd ./CyanOS
make clean && make run
```

### Linux (Ubuntu)
#### Setting up
Using bash, type the following commands to install the required packages
```
sudo apt-get update
sudo apt-get install build-essential gcc-multilib g++-multilib nasm qemu
```
#### Building

```
git clone https://github.com/AymenSekhri/CyanOS.git
cd ./CyanOS
make clean && make run
```
You may need to change the name of Qemu's executable in /kernel/Makefile under the variable `QEMU` to your Qemu's name.
