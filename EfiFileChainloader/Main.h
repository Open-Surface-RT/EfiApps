#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FileHandleLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleTextIn.h>

VOID Tegra3ConsoleOutputFixup(VOID);
EFI_STATUS LoadEfiFile(CHAR16* fileName, UINTN** memoryLocation, UINTN* fileSize);
EFI_STATUS StartEfiFile(UINTN* memoryLocation, UINTN fileSize);