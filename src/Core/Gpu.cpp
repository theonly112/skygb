#include "Gpu.h"

Gpu::Gpu(System* system)
{
	this->system = system;
	this->lastTicks = 0;
	this->tick = 0;
	this->mode = GpuModeHBlank;
	this->scanline = 0;
	this->control = 0;
	this->scrollX = 0;
	this->scrollY = 0;
	memset(this->tiles, 0, sizeof(this->tiles));

	backgroundPalette[0] = palette[0];
	backgroundPalette[1] = palette[1];
	backgroundPalette[2] = palette[2];
	backgroundPalette[3] = palette[3];

	spritePalette[0][0] = palette[0];
	spritePalette[0][1] = palette[1];
	spritePalette[0][2] = palette[2];
	spritePalette[0][3] = palette[3];

	spritePalette[1][0] = palette[0];
	spritePalette[1][1] = palette[1];
	spritePalette[1][2] = palette[2];
	spritePalette[1][3] = palette[3];
}

void Gpu::UpdateTile(u16 address, u8 value)
{
	address &= 0x1ffe;

	u16 tile = (address >> 4) & 511;
	u16 y = (address >> 1) & 7;

	IMemory* memory = system->memory.get();

	const u16 vramOffset = 0x8000;
	u16 memoryAddress = address + vramOffset;

	for (u8 x = 0; x < 8; x++)
	{
		u8 bitIndex = 1 << (7 - x);

		tiles[tile][y][x] = 
			((memory->ReadByte(memoryAddress)	  & bitIndex) ? 1 : 0) + 
		 	((memory->ReadByte(memoryAddress + 1) & bitIndex) ? 2 : 0);
	}
}

void Gpu::Step()
{
	tick += (system->cpu->Ticks - lastTicks);
	lastTicks = system->cpu->Ticks;

	switch (mode)
	{
	case GpuModeHBlank:
		if(tick >= 204)
		{
			HBlank();
			if(scanline == 143)
			{
				if (system->interrupts->enable & InterruptVBlank)
					system->interrupts->flags |= InterruptVBlank;
				mode = GpuModeVBlank;
			}
			else
			{
				mode = GpuModeOAM;
			}
			tick -= 204;
		}
		break;
	case GpuModeVBlank:
		if(tick >= 456)
		{
			scanline++;
			if(scanline > 153)
			{
				scanline = 0;
				mode = GpuModeOAM;
			}
			tick -= 456;
		}
		break;
	case GpuModeOAM:
		if(tick >= 80)
		{
			mode = GpuModeVRAM;
			tick -= 80;
		}
		break;
	case GpuModeVRAM:
		if(tick >= 172)
		{
			mode = GpuModeHBlank;
			system->display->RenderScanline();
			tick -= 172;
		}
		break;
	}
}

void Gpu::HBlank()
{
	scanline++;
}
