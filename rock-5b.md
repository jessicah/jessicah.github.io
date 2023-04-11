# Rock 5B Notes

This is just a collection of links and notes to get various things
working on my Rock 5B device. I'm particularly interested in having
UEFI working, and eventually trying to boot Haiku on it. But also
potentially using it as a Kodi device as well.

## UEFI Related Information

- https://forum.radxa.com/t/uefi-edk2-on-the-rock-5b/1264
- https://forum.radxa.com/t/windows-uefi-on-rock-5-mega-thread/12924
- https://gitlab.com/rk3588_linux/rk/uefi-monorepo/-/commits/master?ref_type=heads
- https://github.com/edk2-porting/edk2-rk35xx

It looks like UEFI HDMI support has been merged to uefi-monorepo, but
not yet incorporated into the edk2-porting repo.

Latest build of Armbian_23.02.3_Rock5b_lunar_current_6.2.0_gnome_desktop
hasn't booted successfully on my device. It starts the Linux kernel,
but eventually triple-faults, I think because of PCIe? I'll need to
revisit the forum thread that mentions issues with PCIe.

## Accelerated KODI

- https://forum.radxa.com/t/rk3588-kodi-rkmpp-accelerated-decoding-working-out-of-box/12785/15
- https://forum.armbian.com/topic/24802-kodi-for-rk35xx-510-legacy-kernel/

I haven't tested any of the above yet.
