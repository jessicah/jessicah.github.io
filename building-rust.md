Building Rust for Haiku
=======================

This is a log of all the steps I've undertaken in order to build [rust](https://www.rust-lang.org/) for
[Haiku](https://www.haiku-os.org/). The hope is to make it easier for others to be able to do the same,
and help work on improving Haiku's support for rust.

```
mkdir scratch && cd scratch
git clone https://github.com/rust-lang/rust --recursive
../build_cross_tools_gcc4 x86_64-unknown-haiku ../haiku ../buildtools cross-haiku -j4
```
