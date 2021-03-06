#include "Main.h"

// Taken from https://github.com/NekomimiRouter/yahallo/blob/main/TegraFirmwareDump/Console.c
VOID Tegra3ConsoleOutputFixup(VOID)
{
	EFI_STATUS                       Status                   = EFI_SUCCESS;
	UINTN                            NumOutputProtocolHandles = 0;
	EFI_HANDLE *                     pOutputHandles;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *pScreenEfiOutputProtocol;

	Status = gBS->LocateHandleBuffer(
	ByProtocol, &gEfiSimpleTextOutProtocolGuid, NULL,
	&NumOutputProtocolHandles, &pOutputHandles);

	// Some sanity check here.
	if (!EFI_ERROR(Status) && NumOutputProtocolHandles >= 3) {
		// Take the last handle as ConOut, don't know how I know this.
		// It is painful...and it is 5am now
		Status = gBS->HandleProtocol(
			pOutputHandles[NumOutputProtocolHandles - 1],
			&gEfiSimpleTextOutProtocolGuid, (VOID **)&pScreenEfiOutputProtocol);

		// Hack: force use the screen output for ConOut
		if (!EFI_ERROR(Status)) {
			gST->ConOut           = pScreenEfiOutputProtocol;
			gST->ConsoleOutHandle = pOutputHandles[NumOutputProtocolHandles - 1];
		}
	}
}