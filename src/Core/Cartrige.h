#ifndef Cartrige_H
#define Cartrige_H
#include "Defines.h"


enum CartrigeType
{
	Plain = 0x00,
	MBC1 = 0x01,
	MBC1RAM = 0x02,
	MBC1RAMBattery = 0x03,
	MBC2 = 0x05,
	MBC2Battery = 0x06,
	ROMRAM = 0x08,
	ROMRAMBattery = 0x09,
	MMM01 = 0x0B,
	MMM01RAM = 0x0C,
	MMM01RAMBattery = 0x0D,
	MBC3TimerBattery = 0x0F,
	MBC3TimerRAMBattery = 0x10,
	MBC3 = 0x11,
	MBC3RAM = 0x12,
	MBC3RAMBattery = 0x13,
	MBC4 = 0x15,
	MBC4RAM = 0x16,
	MBC4RAMBattery = 0x17,
	MBC5 = 0x19,
	MBC5RAM = 0x1A,
	MBC5RAMBattery = 0x1B,
	MBC5Rumble = 0x1C,
	MBC5RumbleRAM = 0x1D,
	MBC5RumbleRAMBattery = 0x1E,
	MBC6 = 0x20,
	MBC7SensorRumbleRAMBattery = 0x22,
	PocketCamera = 0xFC,
	BandaiTama5 = 0xFD,
	HuC3 = 0xFE,
	HuC1RAMBattery = 0xFF
};

struct CartrigeHeader
{
	// http://gbdev.gg8.se/wiki/articles/The_Cartridge_Header
	u8 EntryPoint[0x04];		//0100-0103 - Entry Point
	u8 NinentoLogo[0x30];		//0104-0133 - Nintendo Logo
	u8 Titel[0x10];				//0134-0143 - Title
	//u8 ManufacturerCode[4];	//013F-0142 - Manufacturer Code
	//u8 CGB_Flag;				//0143 - CGB Flag
	u8 NewLicenseeCode[2];		//0144-0145 - New Licensee Code
	u8 SGB_Flag;				//0146 - SGB Flag
	u8 Cartrige_Type;			//0147 - Cartridge Type
	u8 ROM_Size;				//0148 - ROM Size
	u8 RAM_Size;				//0149 - RAM Size
	u8 Destination_Code;		//014A - Destination Code
	u8 Old_Licensee_Code;		//014B - Old Licensee Code
	u8 Mask_ROM_Version_Number;	//014C - Mask ROM Version number
	u8 Header_Checksum;			//014D - Header Checksum
	u8 Global_Checksum[2];		//014E-014F - Global Checksum
};

#endif // Cartrige_H

