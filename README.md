
<p align="center">
  <img width="400" height="80" src="https://i.imgur.com/KVBFGI0.png">
</p>

# CyanOS: A Hobbyist Operating System [![Build Status](https://travis-ci.com/AymenSekhri/CyanOS.svg?branch=master)](https://travis-ci.com/AymenSekhri/CyanOS)

## What's is this ?
It's a x86 monolithic kernel operating system, coded in C++ 17 and few lines of x86 Assembly.

## Why ?
Why a new half working operating systems? you might ask, And the answer: because it's fun. This is my biggest project so far and I'm really learning a lot of new stuff on the way and enjoying every line I write (until I face a race condition bug, I would hate my life by then). I'm trying my best to work out a clean architecture for this project and maintaining a readable & scalable code base as far as i can (it might far from perfect right now).

## What can it do ?
- Virtual Memory.
- Heap Allocator.
- Concurrency in terms of Threads & Processes.
- Basic Windows PE loader
- Task Synchronization (Spinlocks, Semaphores and Mutex)
- Inter-process communication (using pipes)
- Virtual File System.
- User Space.
- PIC Driver.
- Keyboard Driver.

## Building CyanOS
#### Requirements
```
sudo apt-get update
sudo apt-get install build-essential g++-multilib nasm python cmake grub2 xorriso mtools qemu 
```
For Windows users, you can build and run the system on WSL1/WSL2 (Windows Subsystem for Linux). Or you can build it using [msys2](http://repo.msys2.org/distrib/x86_64/), [nasm](https://www.nasm.us/) and [CMake](https://cmake.org/download) and run it using [qemu](https://www.qemu.org/download/), though it still needs WSL to make a bootable ISO using `grub2-rescue` command.

#### Building
```
git clone --recursive https://github.com/AymenSekhri/CyanOS.git
cd ./CyanOS
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../Toolchain.i686-pe.cmake -G "Unix Makefiles"
make && make test
```
And you can boot up the OS in Qemu using:
```
make run
```

## You have a question or suggestion ?
Add an issue, and I will be happy to answer you.

## Acknowledgement
I would like to thank [Andreas Kling](https://github.com/awesomekling), I leant so much from his [youtube channel](https://www.youtube.com/c/AndreasKling/) and his great open source project [SerenityOS](https://github.com/SerenityOS/serenity).

## Useful resources
* Operating Systems: Internals And Design Principles By William Stallings.
* Operating Systems: Design and Implementation By Albert S. Woodhull and Andrew S. Tanenbaum.
* [Brokenthorn Tutorials](http://www.brokenthorn.com/Resources)
* [littleosbook Tutorials](https://littleosbook.github.io)
* [James Molloy Tutorials](http://www.jamesmolloy.co.uk/tutorial_html)
* [How to Make a Computer Operating System](https://samypesse.gitbook.io/how-to-create-an-operating-system/)
* [OSDev](https://wiki.osdev.org/Main_Page)
* [Intel Manual](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.pdf)
