# Where all the UEFI development lives...

These are some notes for working on UEFI code, where to find things, etc.

OVMF: <https://sourceforge.net/projects/edk2/files/OVMF/>

## Building a UEFI FTDI Driver

I use an FTDI-based USB to serial cable for serial debugging of Haiku on hardware. This has some support in EDK II, and with the combination of rEFInd <http://www.rodsbooks.com/refind/>, which will load the driver automatically for us, allows early debugging of UEFI for the Haiku Loader. This will also extend to kernel debugging, which will be able to continue to use the USB serial port for debugging using traditional port i/o.

    sudo apt-get install acpica-tools nasm uuid-dev
    git clone https://github.com:/tianocore/edk2
    cd edk2
    make -C BaseTools
    . edksetup.sh
    nano -w Conf/target.txt
    build

Options to go into target.txt:

- ACTIVE_PLATFORM = OptionRomPkg/OptionRomPkg.dsc
- TARGET_ARCH = X64
- TOOL_CHAIN_TAG = GCC5

Further information:
- <https://github.com/tianocore/edk2>
- <https://github.com/tianocore/tianocore.github.io/wiki/Using-EDK-II-with-Native-GCC>

## Building the Haiku Loader

    git clone https://git.haiku-os.org/buildtools
    git clone https://git.haiku-os.org/haiku
    cd haiku
    ./configure --build-cross-tools x86_64 ../buildtools --use-gcc-pipe -j4
    HAIKU_BOOT_PLATFORM=efi jam -q haiku_loader.efi

`HAIKU_BOOT_PLATFORM` must always be specified when building haiku_loader.efi. If exported, you will need to use `HAIKU_BOOT_PLATFORM=bios_ia32` when building a standard Haiku image.

## Testing with QEMU

This assumes you've built an anyboot image, e.g. `jam -q -j4 @nightly-anyboot`. Support for other image types are not yet supported, nor other devices. If you'd like to use a USB disk, see the patch further down.

    cd haiku/generated
    qemu-system-x86_64 -m 256 -vnc :0 -hda fat:objects/haiku/x86_64/release/system/boot/efi -cdrom haiku-nightly-anyboot.iso -bios OVMF.fd -net none -no-reboot -serial stdio

## Building a Bootable USB Disk

First, you need to apply the `devices.cpp` portion of the patch listed in Other Notes. Probably the easiest approach is to boot a VM with QEMU and a raw disk image, and then `dd` it to your USB disk.

    dd if=/dev/zero of=raw-disk-image bs=1M count=1000 # adjust count as necessary; this creates a 1GB image
    qemu-system_x86_64 -m 512 -hda haiku-nightly.image -hdb raw-disk-image -hdc fat:objects/haiku/x86_64/release/system/boot/efi

The following steps in Haiku should prepare the disk image for you:

1. In DriveSetup, create a new GPT partitioning system
2. Create two partitions, first is EFI System Data, second is BFS
3. Format EFI with FAT File System, name efi, use defaults
4. Format BFS, use defaults
5. Mount all unmounted partitions
6. Make EFI/BOOT on the EFI system partition (efi) and copy haiku_loader to EFI/BOOT and rename to BOOTX64.efi
7. Run Installer, and install your running Haiku to the new BFS partition
8. Shutdown, and your raw disk image should be complete!

## Other Notes

The following gist fixes a couple issues with setting up the frame buffer, and lets you boot off a USB disk: <https://gist.github.com/jessicah/6c993f46556048b44fd710cdbc81bd50>

Note however, that only the devices.cpp part is needed, also at `gBootVolume.SetBool(BOOT_VOLUME_BOOTED_FROM_IMAGE, true);` this should also be set to false, which is missing in the patch. Probably easiest to just manually apply the changes to devices.cpp yourself.

And serial debugging: <https://gist.github.com/jessicah/fa26155c8e3236d9e4ede64f7105d90a> (now in master)
