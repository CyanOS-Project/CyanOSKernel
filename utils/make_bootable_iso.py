import sys
import os
import platform
import shutil
import tarfile

# make_bootable_iso.py krnl_bin_dir apps_bin_dir root_dir iso_out
# krnl_bin_dir: path of the compiled kernel directory.
# apps_bin_dir: path of the apps folder to create ramdisk for.
# root_dir: path of the folder where to create the ramdisk hierarchy.
# iso_out: path of output iso


def main():
    if(len(sys.argv) != 5):
        print("invalid argument list")
        sys.exit(2)
    krnl_bin_dir = os.path.relpath(sys.argv[1])
    apps_bin_dir = os.path.relpath(sys.argv[2])
    root_dir = os.path.relpath(sys.argv[3])
    iso_out = os.path.relpath(sys.argv[4])
    if os.path.exists(root_dir) == True:
        shutil.rmtree(root_dir)
    make_root(krnl_bin_dir, root_dir)
    create_ramdisk(apps_bin_dir, os.path.join(root_dir, "ramdisk.tar"))
    make_iso(root_dir, iso_out)


def make_iso(folder_name, out_path):
    if platform.system() == "Windows":
        folder_name = folder_name.replace(os.sep, '/')
        out_path = out_path.replace(os.sep, '/')
        os.system("wsl grub-mkrescue -o " + out_path + " " + folder_name)
    elif platform.system() == "Linux":
        os.system("grub-mkrescue -o " + out_path + " " + folder_name)
    else:
        print("Not supported system")
        sys.exit(2)


def make_root(bin_path, out_dir):
    os.makedirs(out_dir)
    os.makedirs(os.path.join(out_dir, "boot/grub/"))
    make_grub_cfg(os.path.join(out_dir, "boot/grub/grub.cfg"),
                  "/boot/kernel.img")
    shutil.copyfile(os.path.join(bin_path, "kernel.img"),
                    os.path.join(out_dir, "boot/kernel.img"))


def create_ramdisk(input_path, output_path):
    tar = tarfile.open(output_path, "w")
    for filename in os.listdir(input_path):
        tar.add(os.path.join(input_path, filename), filename)
    tar.close()


def make_grub_cfg(cfg_path, kernel_path):

    f = open(cfg_path, "w")
    f.write('set timeout=0\n')
    f.write('set default=0\n')
    f.write('menuentry "CyanOS" {\n')
    f.write('        multiboot2 ' + kernel_path + '\n')
    f.write('        module2 /ramdisk.tar ramdisk\n')
    f.write('}')


if __name__ == "__main__":
    main()
