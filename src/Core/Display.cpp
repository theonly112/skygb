#include "Display.h"
#include "Gpu.h"
#include "SDL.h"
#include "SDL_opengl.h"

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

Display::Display(System* system, SDL_Window* window) : system(system), window(window)
{
	memset(framebuffer, 255, sizeof(framebuffer));
	m_pSpriteXCacheBuffer = new int[160 * 144];
	m_pColorCacheBuffer = new u8[160 * 144];
	memset(m_pColorCacheBuffer, 0, sizeof(m_pColorCacheBuffer));
	memset(m_pSpriteXCacheBuffer, 0, sizeof(m_pSpriteXCacheBuffer));
}

void Display::DrawFramebuffer() const
{
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2f(-1, 1);
	glPixelZoom(1, -1);
	glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);
	SDL_GL_SwapWindow(window);
	//Sleep(1000/60);
}

//void Display::RenderScanline()
//{
//	Gpu* gpu = system->gpu.get();
//
//	int mapOffset = (gpu->control & GPU_CONTROL_TILEMAP) ? 0x1c00 : 0x1800;
//	mapOffset += (((gpu->scanline + gpu->scrollY) & 255) >> 3) << 5;
//
//	int lineOffset = (gpu->scrollX >> 3);
//	int x = gpu->scrollX & 7;
//	int y = (gpu->scanline + gpu->scrollY) & 7;
//
//	int pixelOffset = gpu->scanline * 160;
//
//	u16 tile = static_cast<u16>(system->memory->GetVRam()[mapOffset + lineOffset]);
//
//	u8 scanlineRow[160];
//	int i;
//	for (i = 0; i < 160; i++) {
//		u8 colour = system->gpu->tiles[tile][y][x];
//		scanlineRow[i] = colour;
//		framebuffer[pixelOffset].r = system->gpu->backgroundPalette[colour].r;
//		framebuffer[pixelOffset].g = system->gpu->backgroundPalette[colour].g;
//		framebuffer[pixelOffset].b = system->gpu->backgroundPalette[colour].b;
//
//		pixelOffset++;
//		x++;
//		if (x == 8) {
//			x = 0;
//			lineOffset = (lineOffset + 1) & 31;
//			tile = system->memory->GetVRam()[mapOffset + lineOffset];
//		}
//	}
//
//	for (i = 0; i < 40; i++)
//	{
//		sprite s = reinterpret_cast<sprite*>(system->memory->GetOam())[i];
//
//		int sx = s.x - 8;
//		int sy = s.y - 16;
//
//		if (sy <= gpu->scanline && (sy + 8) > gpu->scanline) {
//			COLOUR *pal = gpu->spritePalette[s.options.palette];
//			pixelOffset = gpu->scanline * 160 + sx;
//
//			u8 tileRow;
//			if (s.options.vFlip) tileRow = 7 - (gpu->scanline - sy);
//			else tileRow = gpu->scanline - sy;
//			int xx;
//			for (xx = 0; xx < 8; xx++) {
//				if (sx + xx >= 0 && sx + xx < 160 && (~s.options.priority || !scanlineRow[sx + xx])) {
//					u8 colour;
//					if (s.options.hFlip) colour = system->gpu->tiles[s.tile][tileRow][7 - xx];
//					else colour = system->gpu->tiles[s.tile][tileRow][xx];
//					if (colour) {
//						framebuffer[pixelOffset].r = pal[colour].r;
//						framebuffer[pixelOffset].g = pal[colour].g;
//						framebuffer[pixelOffset].b = pal[colour].b;
//					}
//					pixelOffset++;
//
//				}
//			}
//		}
//	}
//}


void Display::RenderScanline()
{
	Gpu* gpu = system->gpu.get();

	RenderBackground(gpu->scanline);
	//RenderWindow(gpu->scanline);
	RenderSprites(gpu->scanline);

}


void Display::RenderBackground(int line)
{
	auto memory = system->memory;
	auto gpu = system->gpu;

	u8 lcdc = memory->ReadByte(0xFF40);
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

void Display::RenderWindow(int line)
{
	if (m_iWindowLine > 143)
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
	int lineAdjusted = m_iWindowLine;
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
			m_pColorCacheBuffer[position] = pixel & 0x03;

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
	m_iWindowLine++;
}

void Display::RenderSprites(int line)
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
