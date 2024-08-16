# Clang with Haiku Cross-Compiler & Sysroot

Just a quick personal note for myself for using clang with a gcc cross-compiler and Haiku sysroot.

Once a cross-compiler and sysroot has been set up, see https://github.com/jessicah/cross-compiler,
then the following options should allow clang to build Haiku binaries:

- `--target=x86_64-unknown-haiku`
- `--sysroot=/home/jessi/source/toolchains`
- `--gcc-toolchain=/home/jessi/source/toolchains/generated/cross-tools-x86_64`

Where `/home/jessi/source/toolchains` contains the sysroot
(i.e. `/home/jessi/source/toolchains/boot/system` exists and contains a Haiku layout), and the
gcc cross-compiler also exists.
