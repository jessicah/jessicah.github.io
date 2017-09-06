Building Rust for Haiku
=======================

This is a log of all the steps I've undertaken in order to build [rust](https://www.rust-lang.org/) for
[Haiku](https://www.haiku-os.org/). The hope is to make it easier for others to be able to do the same,
and help work on improving Haiku's support for rust.

Patches Not Upstreamed
----------------------

For easy reference, so I know what's missing and need to be aware of!

- [openssl-probe search paths](https://github.com/jessicah/rust-openssl-probe/commit/20d3cbbebeeb2c6636375505a968ee467c507887
- [ssh2 target dirs](https://github.com/jessicah/rust-ssh2/commit/6b72784df4c3a5da9b53711ca6d16ad3e543e29e)
- [net2 ipv6 etc](https://github.com/jessicah/rust-net2/commit/a62cb92108aca46dd69a73d24ed678905ee86912)
- [git2 target dirs](https://github.com/jessicah/rust-git2/commit/6f9bdf75fdaeb16d590f2ab781e12ddaa9e16523)
- [openssl target dirs](https://github.com/jessicah/rust-openssl/commit/a165eed9820d9862595cf9b90440963db79697af)
- [cargo homedir](https://github.com/jessicah/rust-cargo/commit/91c9ae2d7fb6bd5d44ee0f04081622454371950f)

And cargo RFC regarding homedir: [rust rfc 1615](https://github.com/rust-lang/rfcs/pull/1615#issuecomment-298463034).

Setting Up Environment
----------------------

We the new availability of a Docker container, we can simplify our life greatly :)

Grab [`rust`](https://github.com/rust-lang/rust), and then run our container:
```
git clone https://github.com:/rust-lang/rust --recursive
docker run -it --name rust-build -v $(pwd)/rust:/rust haiku/cross-compiler:x86_64 bash
```

We should now be running in a shell inside the container. Next we need to grab LLVM and
make it available to our cross-compiler:
```
wget http://packages.haiku-os.org/haikuports/master/hpkg/llvm-4.0.1-2-x86_64.hpkg
wget http://packages.haiku-os.org/haikuports/master/hpkg/llvm_libs-4.0.1-2-x86_64.hpkg
package extract -C /system llvm*.hpkg
```

We also need our shim `llvm-config`:
```
cat >/bin/llvm-config/haiku <<EOF
#!/bin/sh

case $1 in
--version) echo  4.0.1;;
--prefix) echo  /system;;
--bindir) echo  /system/bin;;
--includedir) echo  /system/develop/headers;;
--libdir) echo  /system/develop/lib;;
--cmakedir) echo  /system/develop/lib/cmake/llvm;;
--cppflags) echo  -I/system/develop/headers   -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS;;
--cflags) echo  -I/system/develop/headers  -fPIC -Wall -W -Wno-unused-parameter -Wwrite-strings -Wno-missing-field-initializers -pedantic -Wno-long-long -Wno-comment -Werror=date-time -ffunction-sections -fdata-sections -O3 -DNDEBUG -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS;;
--cxxflags) echo  -I/system/develop/headers  -fPIC -fvisibility-inlines-hidden -Wall -W -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wno-missing-field-initializers -pedantic -Wno-long-long -Wno-maybe-uninitialized -Wdelete-non-virtual-dtor -Wno-comment -Werror=date-time -std=c++11 -ffunction-sections -fdata-sections -O3 -DNDEBUG  -fno-exceptions -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS;;
--ldflags) echo  -L/system/develop/lib ;;
--system-libs) echo ;;
--libs) echo  -lLLVM-4.0;;
--libfiles) echo  /system/develop/lib/libLLVM-4.0.so;;
--components) echo  aarch64 aarch64asmparser aarch64asmprinter aarch64codegen aarch64desc aarch64disassembler aarch64info aarch64utils all all-targets amdgpu amdgpuasmparser amdgpuasmprinter amdgpucodegen amdgpudesc amdgpudisassembler amdgpuinfo amdgpuutils analysis arm armasmparser armasmprinter armcodegen armdesc armdisassembler arminfo asmparser asmprinter bitreader bitwriter bpf bpfasmprinter bpfcodegen bpfdesc bpfdisassembler bpfinfo codegen core coroutines coverage debuginfocodeview debuginfodwarf debuginfomsf debuginfopdb demangle engine executionengine globalisel hexagon hexagonasmparser hexagoncodegen hexagondesc hexagondisassembler hexagoninfo instcombine instrumentation interpreter ipo irreader lanai lanaiasmparser lanaicodegen lanaidesc lanaidisassembler lanaiinfo lanaiinstprinter libdriver lineeditor linker lto mc mcdisassembler mcjit mcparser mips mipsasmparser mipsasmprinter mipscodegen mipsdesc mipsdisassembler mipsinfo mirparser msp430 msp430asmprinter msp430codegen msp430desc msp430info native nativecodegen nvptx nvptxasmprinter nvptxcodegen nvptxdesc nvptxinfo objcarcopts object objectyaml option orcjit passes powerpc powerpcasmparser powerpcasmprinter powerpccodegen powerpcdesc powerpcdisassembler powerpcinfo profiledata riscv riscvcodegen riscvdesc riscvinfo runtimedyld scalaropts selectiondag sparc sparcasmparser sparcasmprinter sparccodegen sparcdesc sparcdisassembler sparcinfo support symbolize systemz systemzasmparser systemzasmprinter systemzcodegen systemzdesc systemzdisassembler systemzinfo tablegen target transformutils vectorize x86 x86asmparser x86asmprinter x86codegen x86desc x86disassembler x86info x86utils xcore xcoreasmprinter xcorecodegen xcoredesc xcoredisassembler xcoreinfo;;
--host-target) echo  x86_64-unknown-haiku;;
--has-rtti) echo  YES;;
--shared-mode) echo  shared;;
esac
EOF
```

And now our work on cross-compiling rust begins:
```
cd /rust
cat >config.toml <<EOF
[llvm]
enabled = true
ninja = true

[build]
host = ["x86_64-unknown-haiku"]
target = ["x86_64-unknown-haiku"]
docs = false
compiler-docs = false
submodules = false

[install]
prefix=/install

[rust]
use-jemalloc = false

[target.x86_64-unknown-haiku]
cc = "x86_64-unknown-haiku-gcc"
cxx = "x86_64-unknown-haiku-g++"
llvm-config = "/bin/llvm-config-haiku"

[dist]
src-tarball = false
EOF
./x.py build
./x.py dist
```

And there you have `rustc` and friends, assuming upstream has our patches merged! If you're making
changes to any sub-modules, you'll also want to add `submodules = false` to the `[build]` section
in `config.toml`, otherwise the build system will try to obliterate your changes.

Unfortunately, compiling `cargo` is quite a different beast, although this is mostly because of the
upstreamed patches mentioned. I've found the easiest solution is to use the cargo config file to
override crates at the top-level.

My `$HOME/.cargo/config` file:
```
paths = [
        "/home/jessicah/vendor/socket2-rs",
        "/home/jessicah/vendor/rust-openssl",
        "/home/jessicah/vendor/rust-git2",
        "/home/jessicah/vendor/rust-ssh2",
        "/home/jessicah/vendor/curl-rust"
]
```

Unfortunately, I also need to rebase my branches, which I haven't done yet. Vendoring the `libc` crate
does **not** work, so don't even try! I found the easiest solution was to just copy the contents of
`src/liblibc` into the copy in `cargo's` registry under `$HOME/.cargo`. Patches for `libc` have been
upstreamed, and hopefully an updated release will land very shortly.

Some Other Notes For Cross-Compiling Crates
-------------------------------------------

E.g, cross-compiling our first library, jemalloc, which won't have support for Haiku...
```
rm -rf build/x86_64-unknown-haiku/native/jemalloc
export TARGET_CC=x86_64-unknown-haiku-gcc
export TARGET_LD=x86_64-unknown-haiku-ld
```

Where did I pick up these magic `TARGET_xyz` environment variables? This is how rust's
[`gcc-rs`](https://github.com/alexcrichton/gcc-rs) module works for configuring host and target compilers. This
is an important enough reference to keep around, so that we can tweak our environment to suit. Eventually, I
think I would put configuration such as this into a shell script that I can source.

Archived Steps
--------------

Old information about [building rust](/building-rust-archived).
