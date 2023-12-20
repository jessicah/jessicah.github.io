# Configuring a Home Assistant VM

## Using `macvtap` Networking

First, create the XML fragment for using `macvtap`:

```xml
<network>
  <name>macvtap-net</name>
  <forward mode="bridge">
    <interface dev="eno1"/>
  </forward>
</network>
```

1. `virsh net-define macvtap-def.xml`
2. `virsh net-autostart macvtap-net`
3. `virsh net-start macvtap-net`

## Installing Home Assistant

`virt-install --name hass --description "Home Assistant Operating System" --ram=3072 --vcpus=2 --import --boot uefi --disk path=/srv/containers/haos_ova-11.2.qcow2,format=qcow2 --network network:macvtap-net,model=virtio --network bridge=virbr0,model=virtio --graphics none --os-variant=generic --hostdev 002.002`

RAM and vCPUs is obviously configurable, and have downloaded the HAOS image already. The `--hostdev` option is to pass through USB devices to the VM, in this instance, my NFC card reader.

Also have two NICs, one for exposing to the local network (`macvtap`), and one for reverse proxying on the host via docker.
