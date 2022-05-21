# Building .Net Core for Haiku

First, a cross-compilation environment is required, which can be done by using my [cross-compiler script](https://github.com/jessicah/cross-compiler):

    export BUILDROOT=$HOME/builds/cross-compiler
    git clone https://github.com/jessicah/cross-compiler
    cd cross-compiler && ./build-rootfs.sh x86_64 --rootfsdir $BUILDROOT

Required Packages:

These have been uploaded to [my website repo](https://github.com/jessicah/jessicah.github.io/tree/master/packages).

All packages here I'm pretty sure will be needed. You can install these with the `package_extract.sh` command
in `$BUILDROOT`:

    for file in *.hpkg ; do $BUILDROOT/package_extract.sh $file ; done

And then .Net Core:

    git clone https://github.com/jessicah/dotnet-runtime
    cd dotnet-runtime
    # make the Haiku cross-compiler available
    export PATH=$PATH:$BUILDROOT/generated/cross-tools-x86_64/bin
    # make the Haiku sysroot available; libraries and headers will be in here
    export ROOTFS_DIR=$BUILDROOT
    git checkout haiku-dotnet7
    ./build.sh clr.runtime -arch x64 -os haiku -c debug -cross
    ./build.sh mono.runtime -arch x64 -os haiku -c debug -cross
    ./build.sh libs.native -arch x64 -os haiku -c debug -cross
    ./build.sh host.native -arch x64 -os haiku -c debug -cross
    
To build the PAL test, run:

    ./build.sh clr.paltests -arch x64 -os haiku -c debug -cross

Then, run the tests on your Haiku machine:

    cd dotnet-runtime
    src/coreclr/pal/tests/palsuite/runpaltests.sh artifacts/bin/coreclr/$(uname).x64.Debug/paltests
