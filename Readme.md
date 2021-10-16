# Efi Apps
This repository contains a few EFI Applications that are useful for the Surface RT & 2 devices.

## EfiFileChainloader
This EFI Application fixes the UEFI's Display output. It then chainloads the `boot.efi` file from the partition the EFI App was loaded from. (e.g. Booted from USB as `<usb-path>/efi/boot/bootarm.efi` and then chainloads `<usb-path>/boot.efi`)

When the chainloaded EFI App returns, this App will exit, so control will be given back to UEFI (or the EFI App that chainloaded this EFI App), which then executes the normal boot process (e.g. Boot Windows from internal storage)

## HelloWorld
This is a simple Hello World EFI Application. It relies on the fact that the standard display output works without issues.

When the "Hello World" message was printend, the file will exit and the previously executed bootloader / EFI App will continue execution (e.g. grub, UEFI firmware, EfiFileChainloader)