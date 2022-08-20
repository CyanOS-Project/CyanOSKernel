
# CyanOS [![Build Status](https://travis-ci.com/AymenSekhri/CyanOS.svg?branch=master)](https://travis-ci.com/AymenSekhri/CyanOS) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/c4b645ecc8a04bad9784f98846e93368)](https://app.codacy.com/gh/AymenSekhri/CyanOS?utm_source=github.com&utm_medium=referral&utm_content=AymenSekhri/CyanOS&utm_campaign=Badge_Grade)

## What's is this ?
It's a x86 monolithic preemptive kernel, coded in C++20 and few lines of x86 Assembly.

## Why ?
Why a new half working operating systems? you might ask, And the answer: because it's fun. This is my biggest project so far and I'm really learning a lot of new stuff on the way and enjoying every line I write (until I face a race condition bug, I would hate my life by then). I'm trying my best to work out a clean architecture for this project and maintaining a readable & scalable code base as far as i can (it might far from perfect right now).

## What can it do ?
- Virtual Memory.
- Heap Allocator.
- Concurrency in terms of Threads & Processes.
- Basic ELF loader
- Task Synchronization (WaitQueues, Spinlocks, Semaphores and Mutex)
- Inter-process communication (using pipes & sockets)
- Virtual File System.
- User Space.
- A Shell & basic commands like cd, ls, cat...
- PCI bus.
- RTL8139 Ethernet Driver.
- Networking (IPv4, UDP, TCP, ARP and DHCP)

## Building CyanOS
#### Requirements
```
sudo apt-get update
sudo apt-get install gcc-multilib g++-multilib build-essential nasm python3 cmake grub2 xorriso mtools qemu 
```
For Windows users, you can build and run the system on WSL1/WSL2 (Windows Subsystem for Linux).
#### Building Cross compiler GCC/GDB
```
sudo chmod -x build_cross_compiling_tools.sh
sudo ./build_cross_compiling_tools.sh
```

#### Building
First, you need to build cross compiler gcc,  check out how in this [osdev wiki page](https://wiki.osdev.org/GCC_Cross-Compiler).<br>
Then, Follow these commands:
```
git clone --recursive https://github.com/AymenSekhri/CyanOS.git
cd ./CyanOS
mkdir build && cd build
cmake .. -G "Unix Makefiles"
make
```
And you can boot up the OS in Qemu using:
```
make run
```

## You have a question or suggestion ?
Add an issue, and I will be happy to answer you.

## Acknowledgement
I would like to thank [Andreas Kling](https://github.com/awesomekling), I learned so much from his [youtube channel](https://www.youtube.com/c/AndreasKling/) and his great open source project [SerenityOS](https://github.com/SerenityOS/serenity).

## Useful resources
* Operating Systems: Internals And Design Principles By William Stallings.
* Operating Systems: Design and Implementation By Albert S. Woodhull and Andrew S. Tanenbaum.
* [Brokenthorn Tutorials](http://www.brokenthorn.com/Resources)
* [littleosbook Tutorials](https://littleosbook.github.io)
* [James Molloy Tutorials](http://www.jamesmolloy.co.uk/tutorial_html)
* [How to Make a Computer Operating System](https://samypesse.gitbook.io/how-to-create-an-operating-system/)
* [OSDev](https://wiki.osdev.org/Main_Page)
* [Intel Manual](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.pdf)
