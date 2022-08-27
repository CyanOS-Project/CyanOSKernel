#pragma once

#include <types.h>

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16
#define IMAGE_NT_SIGNATURE               0x00004550
#define IMAGE_DOS_SIGNATURE              0x5A4D
#define IMAGE_NT_OPTIONAL_HDR32_MAGIC    0x10b
#define IMAGE_SIZEOF_SHORT_NAME          8

#pragma pack(1)
typedef struct _IMAGE_DOS_HEADER {
	u16 e_magic;
	u16 e_cblp;
	u16 e_cp;
	u16 e_crlc;
	u16 e_cparhdr;
	u16 e_minalloc;
	u16 e_maxalloc;
	u16 e_ss;
	u16 e_sp;
	u16 e_csum;
	u16 e_ip;
	u16 e_cs;
	u16 e_lfarlc;
	u16 e_ovno;
	u16 e_res[4];
	u16 e_oemid;
	u16 e_oeminfo;
	u16 e_res2[10];
	u32 e_lfanew;
} IMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
	u16 Machine;
	u16 NumberOfSections;
	u32 TimeDateStamp;
	u32 PointerToSymbolTable;
	u32 NumberOfSymbols;
	u16 SizeOfOptionalHeader;
	u16 Characteristics;
} IMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
	u32 VirtualAddress;
	u32 Size;
} IMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER {
	u16 Magic;
	uint8_t MajorLinkerVersion;
	uint8_t MinorLinkerVersion;
	u32 SizeOfCode;
	u32 SizeOfInitializedData;
	u32 SizeOfUninitializedData;
	u32 AddressOfEntryPoint;
	u32 BaseOfCode;
	u32 BaseOfData;
	u32 ImageBase;
	u32 SectionAlignment;
	u32 FileAlignment;
	u16 MajorOperatingSystemVersion;
	u16 MinorOperatingSystemVersion;
	u16 MajorImageVersion;
	u16 MinorImageVersion;
	u16 MajorSubsystemVersion;
	u16 MinorSubsystemVersion;
	u32 Win32VersionValue;
	u32 SizeOfImage;
	u32 SizeOfHeaders;
	u32 CheckSum;
	u16 Subsystem;
	u16 DllCharacteristics;
	u32 SizeOfStackReserve;
	u32 SizeOfStackCommit;
	u32 SizeOfHeapReserve;
	u32 SizeOfHeapCommit;
	u32 LoaderFlags;
	u32 NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_NT_HEADERS {
	u32 Signature;
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32;

typedef struct _IMAGE_SECTION_HEADER {
	uint8_t Name[IMAGE_SIZEOF_SHORT_NAME];
	union {
		u32 PhysicalAddress;
		u32 VirtualSize;
	} Misc;
	u32 VirtualAddress;
	u32 SizeOfRawData;
	u32 PointerToRawData;
	u32 PointerToRelocations;
	u32 PointerToLinenumbers;
	u16 NumberOfRelocations;
	u16 NumberOfLinenumbers;
	u32 Characteristics;
} IMAGE_SECTION_HEADER;
#pragma pack()