#!/bin/bash
set -e

BINUTILS_VERSION=2.39
GCC_VERSION=12.2.0
GDB_VERSION=12.1

sudo apt-get install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libgnutls28-dev libisl-dev libexpat1-dev
PREFIX="/opt/cross"
TARGET="i686-elf"
PATH="$PREFIX/bin:$PATH"

cd ~
wget https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.xz
tar -xf binutils-$BINUTILS_VERSION.tar.xz
cd  binutils-$BINUTILS_VERSION/
mkdir -p build-binutils
cd build-binutils
../configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install
rm -r ~/binutils-$BINUTILS_VERSION*


cd ~
wget https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-$GCC_VERSION.tar.xz
tar -xf gcc-$GCC_VERSION.tar.xz
cd gcc-$GCC_VERSION
mkdir -p build-gcc
cd build-gcc
../configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
rm -r ~/gcc-$GCC_VERSION*


cd ~
wget https://ftp.gnu.org/gnu/gdb/gdb-$GDB_VERSION.tar.xz
tar -xf gdb-$GDB_VERSION.tar.xz
cd gdb-$GDB_VERSION
mkdir -p gdb-build
cd gdb-build
../configure --target="$TARGET" --prefix="$PREFIX"  --with-expat
make
make install
rm -r ~/gdb-$GDB_VERSION*