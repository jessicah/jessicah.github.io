Building Rust for Haiku
=======================

This is a log of all the steps I've undertaken in order to build [rust](https://www.rust-lang.org/) for
[Haiku](https://www.haiku-os.org/). The hope is to make it easier for others to be able to do the same,
and help work on improving Haiku's support for rust.

We'll set up our system, doing everything relative to scratch:
```
mkdir -p $HOME/scratch
export SCRATCH=$HOME/scratch
cd $SCRATCH
```

First, we need to build our cross-compiler:
```
git clone https://github.com:/haiku/haiku.git
git clone https://github.com:/haiku/buildtools.git
wget https://gist.githubusercontent.com/jessicah/5bc1da8f72ace11d77a42335951242e6/raw/0b5a25f45622df8c21397df561e562fed0e5166b/build_cross_tools_gcc4
chmod a+x build_cross_tools_gcc4
mkdir -p haiku-cross
./build_cross_tools_gcc4 x86_64-unknown-haiku ./haiku ./buildtools haiku-cross -j4
export PATH=$SCRATCH/haiku-cross/bin:$PATH
```

Copying across a Haiku system to build our sysroot:
```
mkdir remote-haiku
mkdir -p haiku-cross/sysroot/boot/system/develop/headers
mkdir -p haiku-cross/sysroot/boot/system/develop/lib
mkdir -p haiku-cross/sysroot/boot/system/develop/tools
sudo sshfs -o follow_symlinks,allow_other,IdentityFile=$HOME/.ssh/id_rsa user@$IP_ADDRESS:/ remote-haiku
cp -rfP remote-haiku/boot/system/develop/headers/* haiku-cross/sysroot/boot/system/develop/headers/
cp -rfP remote-haiku/boot/system/develop/lib/* haiku-cross/sysroot/boot/system/develop/lib/
cp -rfP remote-haiku/boot/system/develop/tools/* haiku-cross-sysroot/boot/system/develop/tools/
```

And now our work on cross-compiling rust begins:
```
git clone https://github.com/rust-lang/rust --recursive
cd rust
./configure --enable-ninja --disable-dist-src --prefix=$HOME/scratch/install --target=x86_64-unknown-haiku
make
```

This is where things get tricky; we need to start cross-compiling our first library, jemalloc, which won't have
support for Haiku...
```
rm -rf build/x86_64-unknown-haiku/native/jemalloc
export TARGET_CC=x86_64-unknown-haiku-gcc
export TARGET_LD=x86_64-unknown-haiku-ld
make
```

Sure enough, it fails: `fatal error: sys/syscall.h: No such file or directory`. This is going to take some work
to port. Since we'll be making changes to the git submodule, we'll need to tell the build system not to manage
them automatically:
```
chmod +w config.mk
echo 'CFG_DISABLE_MANAGE_SUBMODULES := 1' >> config.mk
```

That's where I'm currently up to. I could of course disable jemalloc, which would make life simpler, but will
tackle that if it's proving difficult to modify...
