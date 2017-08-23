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
cd $SCRATCH/haiku-cross/sysroot/boot/system/develop/lib
ln -s ../tools/lib/gcc/x86_64-unknown-haiku x86_64-unknown-haiku
```

And now our work on cross-compiling rust begins:
```
cd $SCRATCH
git clone https://github.com/rust-lang/rust --recursive
cd rust
./configure --enable-ninja --disable-dist-src --prefix=$HOME/scratch/install --target=x86_64-unknown-haiku --host=x86_64-unknown-haiku
make
```

This is where things get tricky; we need to start cross-compiling our first library, jemalloc, which won't have
support for Haiku...
```
rm -rf build/x86_64-unknown-haiku/native/jemalloc
export TARGET_CC=x86_64-unknown-haiku-gcc
export TARGET_LD=x86_64-unknown-haiku-ld
```

Where did I pick up these magic `TARGET_xyz` environment variables? This is how rust's
[`gcc-rs`](https://github.com/alexcrichton/gcc-rs) module works for configuring host and target compilers. This
is an important enough reference to keep around, so that we can tweak our environment to suit. Eventually, I
think I would put configuration such as this into a shell script that I can source. Anyway, let's continue with
the build...
```
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
tackle that if it's proving difficult to modify... and it turns out just removing the syscall header made it
build fine. Whether it actually works remains to be seen.

So now that `rustc` appears to have compiled successfully, we get to move onto the bigger beast: `cargo`. But first,
let's install our new rust toolchain:
```
make dist
make install
```

This should give us a directory layout that looks like thus:
```
install
  --> bin { rustc, rustdoc, rust-gdb, rust-lldb }
  --> lib
       --> rustlib
            --> x86_64-unknown-haiku
            --> x86_64-unknown-linux-gnu
```

With our toolchain in place, we can tell `cargo` to build with it:
```
export TOOLS=$SCRATCH/install
cd cargo
./configure --prefix=$TOOLS --local-rust-root=$TOOLS --target=x86_64-unknown-haiku
make
```

At this point, it's known the build will eventually fail; there's a lot of modules/crates that need added platform
support for Haiku, but we'll get there...

Other Notes of Importance
-------------------------

A bunch of work not merged upstream:
- [rust-jemalloc](https://github.com/jessicah/rust-jemalloc/tree/haiku-support)
- [rust-fs2](https://github.com/jessicah/rust-fs2/tree/haiku-support)
- [rust-git2](https://github.com/jessicah/rust-git2/tree/haiku-support)
- [rust-ssh2](https://github.com/jessicah/rust-ssh2/tree/haiku-support)
- [rust-openssl](https://github.com/jessicah/rust-openssl/tree/haiku-support)
- [rust-openssl-probe](https://github.com/jessicah/rust-openssl-probe/tree/haiku-support)
- [rust-net2](https://github.com/jessicah/rust-net2/tree/haiku-support)
- [rust-cargo](https://github.com/jessicah/rust-cargo/commit/91c9ae2d7fb6bd5d44ee0f04081622454371950f)

Notes from previous notes and build attempts:

1. Running the `rustc` cross-compiler:
   ```
   LD_LIBRARY_PATH=build/x86_64-unknown-linux-gnu/stage2/lib \
     ./build/x86_64-unknown-linux-gnu/stage2/bin/rustc -v --target x86_64-unknown-haiku hello.rs
   ```
2. Verifying correctly linked binary:
   ```
   readelf -d hello
   # example output
   Dynamic section at offset 0xe4810 contains 22 entries:
     Tag        Type                         Name/Value
   0x0000000000000001 (NEEDED)             Shared library: [libroot.so]
   0x0000000000000001 (NEEDED)             Shared library: [libnetwork.so]
   0x0000000000000001 (NEEDED)             Shared library: [libgcc_s.so.1]
   ```
3. Building the rust cross-compiler:
   ```
   CC=clang CXX=clang++ ./x.py dist --target x86_64-unknown-haiku --host x86_64-unknown-linux-gnu
   # we can also use --host x86_64-unknown-haiku to build a rustc that runs on Haiku
   ```

There are also some other tricks to building the cross-compiler. One of these is to create a shim for `llvm-config`. In
this case, I just create a shell script, copy the output from llvm-config for each of the output flags on Haiku, and
paste these into the shell script, and adjust the paths to be sub-directories of our sysroot.

I still haven't succeeded at cross-compiling `cargo` for Haiku, but from reading up on the `gcc-rs` project, it seems
this should be doable by setting about a dozen environment variables. The [README](https://github.com/alexcrichton/gcc-rs)
goes into sufficient detail about how this works, but I haven't attempted this just yet.

There are a number of modules/crates that need Haiku support added before they'll compile correctly. I find the easiest
approach to this is to checkout the needed repos into a subfolder I called `vendor`, and add a `[replace]` section in the
`Cargo.toml` file, e.g.:
   ```
   [replace]
   "curl:0.4.6" = { path = "vendor/curl-rust" }
   "curl-sys:0.3.10" = { path = "vendor/curl-rust/curl-sys" }
   ```

To avoid `cargo` cross-compiling OpenSSL, we can also run `make` like so:
   ```
   OPENSSL_LIB_DIR=$SYSROOT/boot/system/develop/lib OPENSSL_INCLUDE_DIR=$SYSROOT/boot/system/develop/headers \
     VERBOSE=1 make
   ```

Also, when invoking `configure` for `cargo`, you need to use the `rustc` you cross-compiled, as it checks that the compiler
is the same build as used for building the crates. If they're not, it will complain that they're not compatible.
