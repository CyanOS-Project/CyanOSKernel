

# Building the required tools for CyanOS

## Setup
```
sudo apt-get install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libcloog-isl-dev libisl-dev
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
```


## Building binutil
```
mkdir build-binutils
cd build-binutils
../configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install
```

## Building gcc

```
mkdir build-gcc
cd build-gcc
../configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
```


## Building gdb

```
mkdir gdb-build
cd gdb-build
../configure --target="$TARGET" --prefix="$PREFIX"  --with-expat
```
