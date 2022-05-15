# Building .Net Core for Haiku

First, a cross-compilation environment is required, which can be done by using my [cross-compiler script](https://github.com/jessicah/cross-compiler):

    export BUILDROOT=$HOME/builds/cross-compiler
    git clone https://github.com/jessicah/cross-compiler
    cd cross-compiler && ./build-rootfs.sh x86_64 --rootfs-dir $BUILDROOT

And then .Net Core:

    git clone https://github.com/jessicah/dotnet-runtime
    cd dotnet-runtime
    # make the Haiku cross-compiler available
    export PATH=$PATH:$BUILDROOT/generated/cross-tools-x86_64/bin
    # make the Haiku sysroot available; libraries and headers will be in here
    export ROOTFS_DIR=$BUILDROOT/boot/system
    ./build.sh clr.runtime -arch x64 -os haiku -c debug -cross --ninja
