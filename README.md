<center><img width="400" height="80" alt="centered image" src="https://i.imgur.com/KVBFGI0.png"></center>

# CyanOS: A Hobbyist Operating System [![Build Status](https://travis-ci.com/AymenSekhri/CyanOS.svg?branch=master)](https://travis-ci.com/AymenSekhri/CyanOS)

## Todo list
- [x] Paging and virtual memory.
- [x] Higher half kernel.
- [x] Interrupt dispatchers.
- [x] Dynamic virtual/physical memory allocation.
- [x] Heap allocator.
- [x] PIC.
- [x] Multi-threading.
- [x] Mutex/Semaphores.
- [x] Multi-programming.
- [ ] User space.
- [ ] System calls.
- [ ] Inter-process communication.
- [ ] Filesystem.
- [ ] Keyboard driver.
- [ ] PCI bus.
- [ ] GUI.
- [ ] Network Driver.
- [ ] USB.

## Building CyanOS
#### Requirement
```
sudo apt-get update
sudo apt-get install build-essential g++-multilib nasm python cmake grub2 xorriso qemu
```
For Windows users, you can build and run the system on WSL(Windows Subsystem for Linux). Or you can use build it using [MinGW32](https://osdn.net/projects/mingw/releases/), [nasm](https://www.nasm.us/) and [CMake](https://cmake.org/download) and run it using [qemu](https://www.qemu.org/download/), though it still needs WSL make bootable ISO using `grub2-rescue`.

#### Building
```
git clone https://github.com/AymenSekhri/CyanOS.git
cd ./CyanOS
mkdir build && cd build
cmake ..
make && make test
```
And you can boot up the OS in Qemu using:
```
make run
```


## Useful resources:
* Operating Systems: Internals And Design Principles By William Stallings.
* Operating Systems: Design and Implementation By Albert S. Woodhull and Andrew S. Tanenbaum.
* http://www.brokenthorn.com/Resources
* https://littleosbook.github.io
* http://www.jamesmolloy.co.uk/tutorial_html
* https://samypesse.gitbook.io/how-to-create-an-operating-system/
* https://wiki.osdev.org/Main_Page
* https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.pdf
