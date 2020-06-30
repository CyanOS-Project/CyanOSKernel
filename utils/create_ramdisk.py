import sys
import os


def main():
    if(len(sys.argv) != 2):
        print("invalid argument list")
        sys.exit(2)
    build_path = sys.argv[1]
    kernel_path = build_path + "/bin/kernel.img"
    ramdisk_path = build_path + "/bin/ramdisk.tar"
    kernel_size = os.path.getsize(kernel_path)
    padding_size = 4096 - kernel_size % 4096
    f = open(kernel_path, "ab")
    #f.write(bytearray('\0' * padding_size, 'utf8'))
    f2 = open(ramdisk_path, 'rb')
    f.write(f2.read())
    f2.close()
    f.close()


if __name__ == "__main__":
    main()
