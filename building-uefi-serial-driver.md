Building UEFI Serial Driver
===========================

This is a quick run-down of how to build the FTDI compatible USB serial driver for UEFI with Ubuntu.

Most of this comes from [the edk2 instructions](https://github.com/tianocore/tianocore.github.io/wiki/Using-EDK-II-with-Native-GCC).

```
sudo apt-get install iasl uuid-dev gcc-5
mkdir src
cd src
git clone https://github.com/tianocore/edk2
cd edk2
make -C BaseTools
. edksetup.sh
sed -i -e 's;IA32;X64;g' Conf/target.txt
sed -i -e 's;Nt32Pkg/Nt32Pkg.dsc;OptionRomPkg/OptionRomPkg.dsc;g' Conf/target.txt
sed -i -e 's;MYTOOLS;GCC5;g' Conf/target.txt
build
find -name '*.efi' | grep -e Ftdi
```
