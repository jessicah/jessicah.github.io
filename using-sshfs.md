Using SSHFS (with Haiku)
========================

Because we're using `sudo`, we have to specify the identify file to use (for pubkey authentication).

```
sudo sshfs -o follow_symlinks,allow_other,IdentityFile=$HOME/.ssh/id_rsa user@$IP_ADDRESS:/ sysroot
```

This will mount a remote installation in the directory sysroot, which is super handy with Haiku.

We can use `pkgman` in our running Haiku installation to add any packages we need.

Using Within Haiku
------------------

In Terminal 1 (leave running in the background):
```
/system/servers/userlandfs_server sshfs
```

In Terminal 2 (can close once done)
```
mkdir /mountpoint
mount -t userlandfs -o 'sshfs nobody@example.com:/path/to/mount' /mountpoint
```
