API Diffing
===========

With rust's `libc` crate, I needed some way to identify parts of the API that might be missing
from Haiku's port, so I used `q: text as data` combined with a bit of `grep` and `awk` to get
an idea of the API surface that should be added.

1. Getting symbols from the combined `unix` flavours:

    grep -rh src/unix/ -E -o -e 'pub fn[\t ][a-zA-Z0-9]+' | awk '{ print $3 }' | sort | uniq > unix.syms

2. Getting symbols from the `haiku` flavour:

    grep -h src/unix/mod.rs src/unix/haiku/mod.rs -E -o -e 'pub fn[\t ][a-zA-Z0-9]+' | awk '{ print $3 }' | sort | uniq > haiku.syms

3. Getting exported symbols from `libroot.so`:

    readelf --dyn-syms -W /boot/system/develop/lib/libroot.so | grep -v -e UND | awk '{ print $8 }' | sort > libroot.syms

4. Running an SQL query for symbols from `unix` that aren't in `haiku`, that exist in `libroot.so`:

    q "SELECT root.c1 FROM libroot.syms root WHERE root.c1 IN (SELECT unix.c1 FROM unix.syms LEFT JOIN haiku.syms ON unix.c1 = haiku.c1 WHERE haiku.c1 IS NULL)"

Et voila! A list of symbols that should potentially be added to the Haiku port in the `libc` crate.
