Collected Notes
===============

Setting up a sysroot for a Haiku cross-compiler
-----------------------------------------------

First off, we need to build the cross-compiler. This is fairly straight-forward, reusing Haiku's build system.

```
git clone https://git.haiku-os.org/buildtools
git clone https://git.haiku-os.org/haiku
cd $HOME/buildtools/jam
make && sudo ./jam0 install
mkdir -p $HOME/haiku/generated
cd $HOME/haiku/generated
../configure --build-cross-tools x86_64 $HOME/buildtools --use-gcc-pipe -jN
mkdir -p cross-tools-x86_64/sysroot
```

Now we have a functioning cross-compiler, but the sysroot isn't set up yet. I've found the easiest way to make
the sysroot is to have a running Haiku system, and utilise `sshfs` to mount our remote system.

```
sudo sshfs -o follow_symlinks,allow_other,IdentityFile=$HOME/.ssh/id_rsa user@$IP_ADDRESS:/ cross-tools-x86_64/sysroot
```

With this approach, it also means that we can install packages with Haiku's `pkgman` for any needed dependencies, rather
than cross-compiling them. For example, installing LLVM instead of cross-compiling it, saving potentially hours building.

We can then add the path to our cross-tools to be able to use them, and build binaries that will run on our Haiku system.

```
export PATH=$PATH:$HOME/haiku/generated/cross-tools-x86_64/bin
```

Cross-Compiling Rust
--------------------

This is just a handful of commands I've found useful to note down whilst cross-compiling Haiku, so I don't forget how to
do in the future...

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
