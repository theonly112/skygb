#ifndef Gpu_H
#define Gpu_H
#include "Defines.h"
#include "System.h"

class System;

enum GpuMode
{
	GpuModeHBlank,
	GpuModeVBlank,
	GpuModeOAM,
	GpuModeVRAM
};

enum GpuControl
{
	GPU_CONTROL_BGENABLE = (1 << 0),
	GPU_CONTROL_SPRITEENABLE = (1 << 1),
	GPU_CONTROL_SPRITEVDOUBLE = (1 << 2),
	GPU_CONTROL_TILEMAP = (1 << 3),
	GPU_CONTROL_TILESET = (1 << 4),
	GPU_CONTROL_WINDOWENABLE = (1 << 5),
	GPU_CONTROL_WINDOWTILEMAP = (1 << 6),
	GPU_CONTROL_DISPLAYENABLE = (1 << 7)
};

class Gpu
{
	System* system;
public:
	u8 tiles[386][8][8];

	COLOUR backgroundPalette[4];
	COLOUR spritePalette[2][4];

	COLOUR framebuffer[160 * 144];

	Gpu(System* system);
	void UpdateTile(uint16_t address, uint8_t value);
	void Step();
	void Reset();
	
	u8 control;
	u8 scrollX;
	u8 scrollY;
	u8 scanline;

	uint32_t tick;
	uint32_t lastTicks;

	GpuMode mode;

	void HBlank();
	
	void RenderScanline();
	void RenderBackground();
	void RenderWindow();
	void RenderSprites();
};

#endif // Gpu_H

