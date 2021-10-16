#include "Main.h"

// Took reference from
// http://git.savannah.gnu.org/cgit/grub.git/tree/grub-core/loader/arm64/linux.c?id=7a210304ebfd6d704b4fc08fe496a0c417441879#n249
// and UEFI Shell source code.


EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS status = EFI_SUCCESS;
	gBS->SetWatchdogTimer(60, 0, 0, NULL);
	if (status != EFI_SUCCESS)
	{
		Print(L"Failed at SetWatchdogTimer!\n");
		goto exit;
	}
	Tegra3ConsoleOutputFixup();
	Print(L"Fixed Display!\nEntering boot.efi...\n");

	// Load efi file into memory
	UINTN* memoryLocation = NULL;
	UINTN fileSize = 0;
	status = LoadEfiFile(L"boot.efi", &memoryLocation, &fileSize);
	if (status != EFI_SUCCESS)
	{
		Print(L"Failed at LoadEfiFile!\n");
		goto exit;
	}
	/*else
		Print(L"LoadEfiFile succeeded!\n");*/

	// LoadImage & StartImage with Source Buffer

	status = StartEfiFile(memoryLocation, fileSize);
	if (status != EFI_SUCCESS)
	{
		Print(L"Failed at StartEfiFile!\n");
		goto exit;
	}
	else
		Print(L"StartEfiFile succeeded!\nExiting...\n");

exit:
	// Let the user inspect the result
	// gBS->Stall(30000000);
	return status;
}

EFI_STATUS StartEfiFile(UINTN* memoryLocation, UINTN fileSize)
{
	EFI_STATUS status;
	EFI_HANDLE newImageHandle;
	/*EFI_LOADED_IMAGE* loadedImage = NULL;
    EFI_GUID loadedImageProtocol = LOADED_IMAGE_PROTOCOL;

	status = gBS->HandleProtocol(
		gImageHandle,
		&loadedImageProtocol, 
		(void **) &loadedImage);

	if (status != EFI_SUCCESS)
	{
		Print(L"Failed at 1st HandleProtocol\n");
		return status;
	}*/

	if (memoryLocation == NULL)
	{
		Print(L"memoryLocation or fileSize is NULL\n");
		return EFI_INVALID_PARAMETER;
	}

	// Print(L"Before LoadImage\n");

	status = gBS->LoadImage(
		TRUE, 
		gImageHandle, 
//		loadedImage->FilePath,
		NULL,
		memoryLocation,
		fileSize, 
		&newImageHandle);

	if (status != EFI_SUCCESS)
	{
		switch (status)
		{
		case EFI_OUT_OF_RESOURCES:
			Print(L"EFI_OUT_OF_RESOURCES\n");
			break;
		case EFI_INVALID_PARAMETER:
			Print(L"EFI_INVALID_PARAMETER\n");
			break;
		case EFI_NOT_FOUND:
			Print(L"EFI_NOT_FOUND\n");
			break;
		case EFI_UNSUPPORTED:
			Print(L"EFI_UNSPPORTED\n");
			break;
		case EFI_LOAD_ERROR:
			Print(L"EFI_LOAD_ERROR\n");
			break;
		case EFI_DEVICE_ERROR:
			Print(L"EFI_DEVICE_ERROR\n");
			break;
		default:
			break;
		}
		Print(L"Failed at LoadImage!\n");
		return status;
	}

	// TODO: Add cmdline to newImageHandle here

	// Print(L"Before StartImage\n");

	status = gBS->StartImage(
		newImageHandle,
		NULL,
		NULL);

	if (status != EFI_SUCCESS)
	{
		Print(L"Failed at StartImage\n");
		return status;
	}

	return status;
}

EFI_STATUS LoadEfiFile(CHAR16* fileName, UINTN** memoryLocation, UINTN* fileSize)
{
	EFI_LOADED_IMAGE* loadedImage = NULL;
    EFI_GUID loadedImageProtocol = LOADED_IMAGE_PROTOCOL;
    EFI_STATUS status;
	EFI_GUID sfspGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fileSystemHandle = NULL;
	EFI_FILE_PROTOCOL* fileSystem = NULL;
	EFI_FILE_PROTOCOL* file = NULL;

	status = gBS->HandleProtocol(
		gImageHandle,
		&loadedImageProtocol, 
		(void **) &loadedImage);

	if (status != EFI_SUCCESS)
	{
		Print(L"Failed at 1st HandleProtocol\n");
		return status;
	}

	status = gBS->HandleProtocol(
		loadedImage->DeviceHandle,
		&sfspGuid,
		(void**)&fileSystemHandle);

	if (status != EFI_SUCCESS)
	{
		Print(L"Failed at 2nd HandleProtocol\n");
		return status;
	}

	status = fileSystemHandle->OpenVolume(fileSystemHandle, &fileSystem);

	if (status != EFI_SUCCESS)
	{
		Print(L"Failed at OpenVolume!\n");
		return status;
	}

	status = fileSystem->Open(
			fileSystem, 
			&file,
			fileName,
			EFI_FILE_MODE_READ,
			EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);

	if (status != EFI_SUCCESS)
	{
		Print(L"Failed at opening file!\n");
		return status;
	}

	EFI_FILE_INFO* fileInfo = NULL;
	UINTN fileInfoSize = 0;

	status = file->GetInfo(
			file,
			&gEfiFileInfoGuid,
			&fileInfoSize,
			(VOID *)fileInfo);

	if (status != EFI_SUCCESS && status != EFI_BUFFER_TOO_SMALL)
	{
		Print(L"Failed at getting file info! (1)\n");
		return status;
	}

	status = gBS->AllocatePool(
		EfiLoaderData, 
		fileInfoSize, 
		(void**)&fileInfo);

	if (status != EFI_SUCCESS)
	{
		Print(L"Failed at allocating pool!\n");
		return status;
	}

	SetMem(
		(VOID *)fileInfo, 
		fileInfoSize, 
		0xFF);

	status = file->GetInfo(
		file,
		&gEfiFileInfoGuid,
		&fileInfoSize,
		(VOID *)fileInfo
	);

	if (status != EFI_SUCCESS)
	{
		Print(L"Failed at getting file info! (2)\n");
		return status;
	}

	Print(L"File size: %llu\n", fileInfo->FileSize);

	status = gBS->AllocatePool(EfiLoaderCode, fileInfo->FileSize, (VOID**)memoryLocation);

	/*UINTN Pages = EFI_SIZE_TO_PAGES (payloadFileSize);
	status = gBS->AllocatePages(AllocateAddress, EfiLoaderData, Pages, &payloadFileBuffer);*/

	if (status != EFI_SUCCESS)
	{
		switch (status)
		{
		case EFI_OUT_OF_RESOURCES:
			Print(L"EFI_OUT_OF_RESOURCES\n");
			break;
		case EFI_INVALID_PARAMETER:
			Print(L"EFI_INVALID_PARAMETER\n");
			break;
		case EFI_NOT_FOUND:
			Print(L"EFI_NOT_FOUND\n");
			break;
		default:
			break;
		}

		Print(L"Failed at AllocatePages!\n");
		return status;
	}

	SetMem(
		(EFI_PHYSICAL_ADDRESS*)*memoryLocation,
		fileInfo->FileSize,
		0xFF);

	status = file->Read(
		file,
		(UINTN*)&fileInfo->FileSize,
		(EFI_PHYSICAL_ADDRESS*)*memoryLocation);

	if (status != EFI_SUCCESS)
	{
		Print(L"Failed at reading file into memory!\n");
		return status;
	}

	Print(L"File is now in memory at location 0x%x!\n", *memoryLocation);
	Print(L"And the address of the pointer is 0x%x!\n", memoryLocation);

	*fileSize = fileInfo->FileSize;

	return EFI_SUCCESS;
}