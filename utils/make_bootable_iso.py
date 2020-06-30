import sys
import os
import platform
import shutil

# make_bootable_iso.py boot_folder output_folder


def main():
    if(len(sys.argv) != 3):
        print("invalid argument list")
        sys.exit(2)
    bin_dir = os.path.relpath(sys.argv[1])
    root_dir = os.path.join(os.path.relpath(sys.argv[2]), "CyanOS_root")
    iso_dir = os.path.join(os.path.relpath(sys.argv[2]), "CyanOS.iso")

    if os.path.exists(root_dir) == True:
        shutil.rmtree(root_dir)
    make_root(bin_dir, root_dir)
    make_iso(root_dir, iso_dir)


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


def create_ramdisk():
    pass


def make_grub_cfg(cfg_path, kernel_path):

    f = open(cfg_path, "w")
    f.write('set timeout=0\n')
    f.write('set default=0\n')
    f.write('menuentry "CyanOS" {\n')
    f.write('        multiboot ' + kernel_path + '\n')
    f.write('}')


pass


if __name__ == "__main__":
    main()
