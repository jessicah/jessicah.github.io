# Modding MyWhoosh

## Tools Used

- AES Keyfinder
- UModel
- Repak
- UE4 DDS Tools GUI
- Blender
- Inkscape

AES Keyfinder was used to get the current key for MyWhoosh: `0x6244C2AC60E556DE3485693B88DB2ADFBD118A76C0737275E74DFE9A83BAE310`

Repak can be used for both unpacking and packing PAK files.

Then UModel for extracting the `.uasset` files, textures and PSK files, etc.

Finally, UE4 DDS Tools is useful for updating `.uasset` files.

This process allows modifying the textures for a model, in my case, to create and test
the Kittyhawk Racing kit.

The main `pak` file containing the avatar textures is located at `C:\Program Files\WindowsApps\MyWhooshTechnologyService.MyWhoosh_5.7.1.0_x64__eps1123pz0kt0\MyWhoosh\Content\Paks\pakchunk0-WindowsNoEditor.pak`.

Unfortunately, because it's a Windows Store app, these files are difficult to modify. Plus
updates can potentially overwrite the `pak` files. It is, however, to run MyWhoosh
unpacked, but it does also mean manually fetching updates.

The MSIX files can be downloaded from https://store.rg-adguard.net/, using the store URL:
`https://apps.microsoft.com/detail/9nvdcph3xdz2?hl=en-US&gl=AE`.

This can then be extracted with 7-zip, and it will run from the extracted folder. However,
you must use `.\MyWhoosh\Binaries\Win64\MyWhoosh.exe` to launch it.

Additional assets will be downloaded to `C:\Users\$USERNAME\AppData\Local\MyWhoosh\Saved\PersistentDownloadDir\PakCache`.

## Basic Texture Colour Replacement

1. Open in GMP
2. Select by Colour
   - Threshold: 49
3. Invert Selection
4. Colorize

Done for gloves, shoes, Liv frame

## Download Links

- https://github.com/atenfyr/UAssetGUI
- https://github.com/trumank/repak
- https://github.com/skarndev/umodel_tools (not used so far)
- https://drive.google.com/file/d/1B6xPMqroOo0WdlYB6suY4VswUE5nrUPO/view (not sure this is the exact one I used, but should be compatible)
- https://github.com/Cracko298/UE4-AES-Key-Extracting-Guide
