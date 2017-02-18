#include "Gpu.h"

Gpu::Gpu(System* system)
{
	memset(framebuffer, 255, sizeof(framebuffer));
	this->system = system;
	Reset();
}

void Gpu::UpdateTile(uint16_t address, uint8_t value)
{
	address &= 0x1ffe;

	uint16_t tile = (address >> 4) & 511;
	uint16_t y = (address >> 1) & 7;

	IMemory* memory = system->memory.get();

	const uint16_t vramOffset = 0x8000;
	uint16_t memoryAddress = address + vramOffset;

	for (uint8_t x = 0; x < 8; x++)
	{
		uint8_t bitIndex = 1 << (7 - x);

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
			RenderScanline();
			tick -= 172;
		}
		break;
	}
}

void Gpu::HBlank()
{
	scanline++;
}

void Gpu::Reset()
{
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

void Gpu::RenderScanline()
{
	RenderBackground();
	//RenderWindow(gpu->scanline);
	RenderSprites();

}


void Gpu::RenderBackground()
{
	auto memory = system->memory;
	auto gpu = system->gpu;

	u8 lcdc = memory->ReadByte(0xFF40);
	auto line = this->scanline;
	int line_width = line * 160;
	if(IsBitSet(lcdc, 0))
	{
		int tiles = IsBitSet(lcdc, 4) ? 0x8000 : 0x8800;
		int map = IsBitSet(lcdc, 3) ? 0x9C00 : 0x9800;
		u8 scx = memory->ReadByte(0xFF43);
		u8 scy = memory->ReadByte(0xFF42);
		u8 lineAdjusted = line + scy;
		int y_32 = (lineAdjusted / 8) * 32;
		int pixely = lineAdjusted % 8;
		int pixely_2 = pixely * 2;
		int pixely_2_flip = (7 - pixely) * 2;

		for (int x = 0; x < 32; x++)
		{
			int tile = 0;

			if (tiles == 0x8800)
			{
				tile = static_cast<int8_t> (memory->ReadByte(map + y_32 + x));
				tile += 128;
			}
			else
			{
				tile = memory->ReadByte(map + y_32 + x);
			}

			int mapOffsetX = x * 8;
			int tile_16 = tile * 16;
			u8 byte1 = 0;
			u8 byte2 = 0;
			int final_pixely_2 = pixely_2;
			int tile_address = tiles + tile_16 + final_pixely_2;

			byte1 = memory->ReadByte(tile_address);
			byte2 = memory->ReadByte(tile_address + 1);
			

			for (int pixelx = 0; pixelx < 8; pixelx++)
			{
				u8 bufferX = (mapOffsetX + pixelx - scx);

				if (bufferX >= 160)
					continue;

				int pixelx_pos = pixelx;


				int pixel = (byte1 & (0x1 << (7 - pixelx_pos))) ? 1 : 0;
				pixel |= (byte2 & (0x1 << (7 - pixelx_pos))) ? 2 : 0;

				int position = line_width + bufferX;

				u8 tmp_palette = memory->ReadByte(0xFF47);
				u8 color = (tmp_palette >> (pixel * 2)) & 0x03;
				framebuffer[position] = gpu->backgroundPalette[color];
			}
		}

	}
}

void Gpu::RenderWindow()
{
	auto line = this->scanline;
	if (this->scanline > 143)
		return;

	auto memory = system->memory;

	u8 lcdc = memory->ReadByte(0xFF40);
	if (!IsBitSet(lcdc, 5))
		return;

	int wx = memory->ReadByte(0xFF4B) - 7;
	if (wx > 159)
		return;

	u8 wy = memory->ReadByte(0xFF4A);
	if ((wy > 143) || (wy > line))
		return;

	int tiles = IsBitSet(lcdc, 4) ? 0x8000 : 0x8800;
	int map = IsBitSet(lcdc, 6) ? 0x9C00 : 0x9800;
	int lineAdjusted = line;
	int y_32 = (lineAdjusted / 8) * 32;
	int pixely = lineAdjusted % 8;
	int pixely_2 = pixely * 2;
	int pixely_2_flip = (7 - pixely) * 2;
	int line_width = (line * 160);

	for (int x = 0; x < 32; x++)
	{
		int tile = 0;

		if (tiles == 0x8800)
		{
			tile = static_cast<int8_t> (memory->ReadByte(map + y_32 + x));
			tile += 128;
		}
		else
		{
			tile = memory->ReadByte(map + y_32 + x);
		}

		//u8 cgb_tile_attr = m_bCGB ? m_pMemory->ReadCGBLCDRAM(map + y_32 + x, true) : 0;
		//u8 cgb_tile_pal = m_bCGB ? (cgb_tile_attr & 0x07) : 0;
		//bool cgb_tile_bank = m_bCGB ? IsSetBit(cgb_tile_attr, 3) : false;
		//bool cgb_tile_xflip = m_bCGB ? IsSetBit(cgb_tile_attr, 5) : false;
		//bool cgb_tile_yflip = m_bCGB ? IsSetBit(cgb_tile_attr, 6) : false;
		//bool cgb_tile_priority = m_bCGB ? IsSetBit(cgb_tile_attr, 7) : false;
		int mapOffsetX = x * 8;
		int tile_16 = tile * 16;
		u8 byte1 = 0;
		u8 byte2 = 0;
		int final_pixely_2 =  pixely_2;
		int tile_address = tiles + tile_16 + final_pixely_2;

		if (false)
		{
			//byte1 = m_pMemory->ReadCGBLCDRAM(tile_address, true);
			//byte2 = m_pMemory->ReadCGBLCDRAM(tile_address + 1, true);
		}
		else
		{
			byte1 = memory->ReadByte(tile_address);
			byte2 = memory->ReadByte(tile_address + 1);
		}
		Gpu* gpu = system->gpu.get();
		auto scx = memory->ReadByte(0xFF43);
		for (int pixelx = 0; pixelx < 8; pixelx++)
		{
			int bufferX = (mapOffsetX + pixelx + wx);
			//int bufferX = (mapOffsetX + pixelx - scx);

			if (bufferX < 0 || bufferX >= 160)
				continue;

			int pixelx_pos = pixelx;

			//if (m_bCGB && cgb_tile_xflip)
			//{
			//	pixelx_pos = 7 - pixelx_pos;
			//}

			int pixel = (byte1 & (0x1 << (7 - pixelx_pos))) ? 1 : 0;
			pixel |= (byte2 & (0x1 << (7 - pixelx_pos))) ? 2 : 0;

			int position = line_width + bufferX;

			if (false)
			{
				//if (cgb_tile_priority && (pixel != 0))
				//	m_pColorCacheBuffer[position] = SetBit(m_pColorCacheBuffer[position], 2);
				//GB_Color color = m_CGBBackgroundPalettes[cgb_tile_pal][pixel];
				//m_pColorFrameBuffer[position] = ConvertTo8BitColor(color);
			}
			else
			{
				u8 tmp_palette = memory->ReadByte(0xFF47);
				u8 color = ((tmp_palette >> (pixel * 2)) & 0x03);
				framebuffer[position] = gpu->backgroundPalette[color];
			}
		}
	}
	
	this->scanline++;
}

void Gpu::RenderSprites()
{
	//auto memory = system->memory;

	//u8 lcdc = memory->ReadByte(0xFF40);

	//if (!IsBitSet(lcdc, 1))
	//	return;

	//int sprite_height = IsBitSet(lcdc, 2) ? 16 : 8;
	//int line_width = (line * 160);

	//for (auto sprite = 39; sprite >= 0; sprite--)
	//{
	//	int sprite_4 = sprite * 4;
	//	int sprite_y = memory->ReadByte(0xFE00 + sprite_4) - 16;

	//	if ((sprite_y > line) || ((sprite_y + sprite_height) <= line))
	//		continue;

	//	int sprite_x = memory->ReadByte(0xFE00 + sprite_4 + 1) - 8;

	//	if ((sprite_x < -7) || (sprite_x >= 160))
	//		continue;

	//	int sprite_tile_16 = (memory->ReadByte(0xFE00 + sprite_4 + 2)
	//		& ((sprite_height == 16) ? 0xFE : 0xFF)) * 16;
	//	u8 sprite_flags = memory->ReadByte(0xFE00 + sprite_4 + 3);
	//	int tmp_sprite_pallette = IsBitSet(sprite_flags, 4) ? 1 : 0;
	//	bool xflip = IsBitSet(sprite_flags, 5);
	//	bool yflip = IsBitSet(sprite_flags, 6);
	//	bool aboveBG = !IsBitSet(sprite_flags, 7);
	//	bool cgb_tile_bank = IsBitSet(sprite_flags, 3);
	//	int cgb_tile_pal = sprite_flags & 0x07;
	//	int tiles = 0x8000;
	//	int pixel_y = yflip ? ((sprite_height == 16) ? 15 : 7) - (line - sprite_y) : line - sprite_y;
	//	u8 byte1 = 0;
	//	u8 byte2 = 0;
	//	int pixel_y_2 = 0;
	//	int offset = 0;

	//	if (sprite_height == 16 && (pixel_y >= 8))
	//	{
	//		pixel_y_2 = (pixel_y - 8) * 2;
	//		offset = 16;
	//	}
	//	else
	//		pixel_y_2 = pixel_y * 2;

	//	int tile_address = tiles + sprite_tile_16 + pixel_y_2 + offset;

	//	if (false)
	//	{
	//		//byte1 = m_pMemory->ReadCGBLCDRAM(tile_address, true);
	//		//byte2 = m_pMemory->ReadCGBLCDRAM(tile_address + 1, true);
	//	}
	//	else
	//	{
	//		byte1 = memory->ReadByte(tile_address);
	//		byte2 = memory->ReadByte(tile_address + 1);
	//	}

	//	for (int pixelx = 0; pixelx < 8; pixelx++)
	//	{
	//		int pixel = (byte1 & (0x01 << (xflip ? pixelx : 7 - pixelx))) ? 1 : 0;
	//		pixel |= (byte2 & (0x01 << (xflip ? pixelx : 7 - pixelx))) ? 2 : 0;

	//		if (pixel == 0)
	//			continue;

	//		int bufferX = (sprite_x + pixelx);

	//		if (bufferX < 0 || bufferX >= 160)
	//			continue;

	//		int position = line_width + bufferX;
	//		u8 color_cache = m_pColorCacheBuffer[position];

	//		if (false)
	//		{
	//			//if (IsSetBit(color_cache, 2))
	//			//	continue;
	//		}
	//		else
	//		{
	//			int sprite_x_cache = m_pSpriteXCacheBuffer[position];
	//			if (IsBitSet(color_cache, 3) && (sprite_x_cache < sprite_x))
	//				continue;
	//		}

	//		if (!aboveBG && (color_cache & 0x03))
	//			continue;

	//		m_pColorCacheBuffer[position] = SetBit(color_cache, 3);
	//		
	//		m_pSpriteXCacheBuffer[position] = sprite_x;
	//		if (false)
	//		{
	//			//GB_Color color = m_CGBSpritePalettes[cgb_tile_pal][pixel];
	//			//m_pColorFrameBuffer[position] = ConvertTo8BitColor(color);
	//		}
	//		else
	//		{
	//			auto gpu = system->gpu;
	//			u8 tmp_palette = memory->ReadByte(tmp_sprite_pallette ? 0xFF49 : 0xFF48);
	//			u8 color = (tmp_palette >> (pixel * 2)) & 0x03;
	//			//framebuffer[position] = palette[color];
	//			framebuffer[position] = gpu->spritePalette[1][color];
	//		}
	//	}
	//}
	
		Gpu* gpu = system->gpu.get();
	
		int mapOffset = (gpu->control & GPU_CONTROL_TILEMAP) ? 0x1c00 : 0x1800;
		mapOffset += (((gpu->scanline + gpu->scrollY) & 255) >> 3) << 5;
	
		int lineOffset = (gpu->scrollX >> 3);
		int x = gpu->scrollX & 7;
		int y = (gpu->scanline + gpu->scrollY) & 7;
	
		int pixelOffset = gpu->scanline * 160;
	
		u16 tile = static_cast<u16>(system->memory->GetVRam()[mapOffset + lineOffset]);


		u8 scanlineRow[160];
		int i;
		for (i = 0; i < 160; i++) {
			u8 colour = system->gpu->tiles[tile][y][x];
			scanlineRow[i] = colour;
	
			pixelOffset++;
			x++;
			if (x == 8) {
				x = 0;
				lineOffset = (lineOffset + 1) & 31;
				tile = system->memory->GetVRam()[mapOffset + lineOffset];
			}
		}

		for (i = 0; i < 40; i++)
		{
			sprite s = reinterpret_cast<sprite*>(system->memory->GetOam())[i];
	
			int sx = s.x - 8;
			int sy = s.y - 16;
	
			if (sy <= gpu->scanline && (sy + 8) > gpu->scanline) {
				COLOUR *pal = gpu->spritePalette[s.options.palette];
				pixelOffset = gpu->scanline * 160 + sx;
	
				u8 tileRow;
				if (s.options.vFlip) tileRow = 7 - (gpu->scanline - sy);
				else tileRow = gpu->scanline - sy;
				int xx;
				for (xx = 0; xx < 8; xx++) {
					if (sx + xx >= 0 && sx + xx < 160 && (~s.options.priority || !scanlineRow[sx + xx])) {
						u8 colour;
						if (s.options.hFlip) colour = system->gpu->tiles[s.tile][tileRow][7 - xx];
						else colour = system->gpu->tiles[s.tile][tileRow][xx];
						if (colour) {
							framebuffer[pixelOffset].r = pal[colour].r;
							framebuffer[pixelOffset].g = pal[colour].g;
							framebuffer[pixelOffset].b = pal[colour].b;
						}
						pixelOffset++;
	
					}
				}
			}
		}

}