# .NET Usage of `libunwind`

The usage of `libunwind` appears to be restricted to `src/coreclr/pal/src/exception`, in
particular, the files `seh-unwind.cpp` and `remote-unwind.cpp`. We can probably ignore
remote unwinding for the meantime.

## Usages in `seh-unwind.cpp`

Types:
- unw_context_t
- unw_cursor_t
- unw_save_loc_t
- unw_word_t

Functions:
- unw_getcontext
- unw_get_reg
- unw_get_save_loc
- unw_init_local
- unw_is_signal_frame
- unw_set_reg
- unw_step

## Usages in `seh-remote.cpp`

Types:
- unw_accessors_t
- unw_addr_space_t
- unw_context_t
- unw_cursor_t
- unw_fpreg_t
- unw_proc_info_t
- unw_regnum_t
- unw_save_loc_t
- unw_word_t

Functions:
- unw_create_addr_space
- unw_destroy_addr_space
- unw_get_reg
- unw_get_save_loc
- unw_init_remote
- unw_step

## Notes

It's quite apparent that skipping remote support will simplify what's needed in Haiku's initial
port of `libunwind`. I think that probably the biggest blocker for the moment is `unw_step`, as
I seem to recall it relying on loading the DWARF debug info regions, which we don't yet have
code for. Currently, that is supposed to be fetched by the `dl_iterate_phdr` function, which
Haiku has no implementation of. Probably the best way forward is to skip `dl_iterate_phdr`
entirely, and provide a Haiku-specific alternative to locating the DWARF debug information, as
`runtime_loader` should load the debug info into specific address space, so we shouldn't need
to search for it, unlike on other platforms.
