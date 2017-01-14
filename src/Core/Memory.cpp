#include "Memory.h"
#include "System.h"
#include <iostream>

void Memory::Copy(uint16_t destination, uint16_t source, uint16_t length)
{
	for (uint16_t i = 0; i < length;i++)
	{
		WriteByte(destination + i, ReadByte(source + i));
	}
}

uint8_t* Memory::GetOam()
{
	return this->oam;
}

uint8_t* Memory::GetVRam()
{
	return this->vram;
}

Memory::Memory(System* system) : system(system)
{
	memset(this->vram, 0, sizeof(vram));
	memset(this->hram, 0, sizeof(hram));
	memcpy(this->io, ioReset, sizeof(io));
	memset(this->sram, 0, sizeof(sram));
	memset(this->wram, 0, sizeof(wram));
	memset(this->oam, 0, sizeof(oam));;
}

uint8_t Memory::ReadByte(uint16_t addr)
{
	if (addr <= 0x7fff)
	{
		return mbc->ReadByte(addr);
	}
	if (addr >= 0xA000 && addr <= 0xBFFF)
	{
		return mbc->ReadByte(addr);
	}
	if (addr >= 0x8000 && addr <= 0x9FFF)
	{
		return vram[addr - 0x8000];
	}
	if (addr >= 0xC000 && addr <= 0xDFFF)
	{
		return wram[addr - 0xC000];
	}
	if (addr >= 0xE000 && addr <= 0xFDFF)
	{
		return wram[addr - 0xE000];
	}
	if (addr >= 0xFE00 && addr <= 0xFEFF)
	{
		return oam[addr - 0xFE00];
	}
	if (addr == 0xff04) return 0;// static_cast<u8>(rand());
	if (addr == 0xff40) return system->gpu->control; 
	if (addr == 0xff42) return system->gpu->scrollY;
	if (addr == 0xff43) return system->gpu->scrollX; 
	if (addr == 0xff44) 
		return system->gpu->scanline;
	if (addr == 0xff00)
	{
		if(!(io[0x00] & 0x20))
		{
			auto val = static_cast<u8>(0xC0 | system->keys.keys1Value | 0x10);
			//std::cout << static_cast<int>(val) << "\n";
			return val;
		}
		if(!(io[0x00] & 0x10))
		{
			auto val = static_cast<u8>(0xC0 | system->keys.keys2Value | 0x20); 
			//std::cout << static_cast<int>(val) << "\n";
			return val;
		}
		if(!(io[0x00] & 0x30))
		{
			return 0xff;
		}
		return 0;
		//return 47;
	}
	if (addr == 0xff0f) return system->interrupts->flags;
	if (addr == 0xffff) return system->interrupts->enable;
	if (addr >= 0xff80 && addr <= 0xfffe)
	{
		return hram[addr - 0xff80];
	}
	if (addr >= 0xff00 && addr <= 0xff7f)
	{
		return io[addr - 0xff00];
	}

	return 0;
}

void Memory::WriteByte(uint16_t addr, uint8_t value)
{
	if(addr <= 0x7FFF)
	{
 		mbc->WriteByte(addr, value);
	}
	if (addr >= 0xA000 && addr <= 0xBFFF)
	{
		mbc->WriteByte(addr, value);
		//sram[addr - 0xA000] = value;
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF)
	{
		vram[addr - 0x8000] = value;
		if (addr <= 0x97FF)
		{
			system->gpu->UpdateTile(addr, value);
		}
	}

	if (addr >= 0xC000 && addr <= 0xDFFF)
	{
		wram[addr - 0xC000] = value;
	}
	else if (addr >= 0xE000 && addr <= 0xFDFF)
	{
		wram[addr - 0xE000] = value;
	}
	else if (addr >= 0xFE00 && addr <= 0xFEFF)
	{
		oam[addr - 0xFE00] = value;
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFE)
	{
		hram[addr - 0xFF80] = value;
	}
	else if (addr == 0xff40) { system->gpu->control = value; }
	else if (addr == 0xff42) { system->gpu->scrollY= value; }
	else if (addr == 0xff43) { system->gpu->scrollX = value; }
	else if (addr == 0xff46) { Copy(0xFE00, value << 8, 160); }

	else if (addr == 0xff47)
	{
		for (int i = 0; i < 4; i++)
		{
			system->gpu->backgroundPalette[i] = palette[(value >> (i * 2)) & 3];
		}
	}
	else if (addr == 0xff48)
	{
		for (int i = 0; i < 4; i++)
		{
			system->gpu->spritePalette[0][i] = palette[(value >> (i * 2)) & 3];
		}
	}
	else if( addr == 0xff49)
	{
		for (int i = 0; i < 4; i++)
		{
			system->gpu->spritePalette[1][i] = palette[(value >> (i * 2)) & 3];
		}
	}
	else if (addr == 0xff0f) { system->interrupts->flags = value; }
	else if (addr >= 0xff00 && addr <= 0xff7f)
	{
		io[addr - 0xff00] = value;
	}
	else if (addr == 0xffff) { system->interrupts->enable = value; }
	//std::cout << "Invalid write " << std::hex << addr << std::endl;
}

uint16_t Memory::ReadWord(uint16_t addr)
{
		u8 low = ReadByte(addr);
		u8 high = ReadByte(addr + 1);

	return (high << 8) | low;
}

void Memory::WriteWord(uint16_t addr, uint16_t value)
{
	WriteByte(addr	  , static_cast<u8>(value & 0x00ff));
	WriteByte(addr + 1, static_cast<u8>((value & 0xff00) >> 8));
}

void Memory::Reset()
{
	WriteByte(0xFF05, 0);
	WriteByte(0xFF06, 0);
	WriteByte(0xFF07, 0);
	WriteByte(0xFF10, 0x80);
	WriteByte(0xFF11, 0xBF);
	WriteByte(0xFF12, 0xF3);
	WriteByte(0xFF14, 0xBF);
	WriteByte(0xFF16, 0x3F);
	WriteByte(0xFF17, 0x00);
	WriteByte(0xFF19, 0xBF);
	WriteByte(0xFF1A, 0x7A);
	WriteByte(0xFF1B, 0xFF);
	WriteByte(0xFF1C, 0x9F);
	WriteByte(0xFF1E, 0xBF);
	WriteByte(0xFF20, 0xFF);
	WriteByte(0xFF21, 0x00);
	WriteByte(0xFF22, 0x00);
	WriteByte(0xFF23, 0xBF);
	WriteByte(0xFF24, 0x77);
	WriteByte(0xFF25, 0xF3);
	WriteByte(0xFF26, 0xF1);
	WriteByte(0xFF40, 0x91);
	WriteByte(0xFF42, 0x00);
	WriteByte(0xFF43, 0x00);
	WriteByte(0xFF45, 0x00);
	WriteByte(0xFF47, 0xFC);
	WriteByte(0xFF48, 0xFF);
	WriteByte(0xFF49, 0xFF);
	WriteByte(0xFF4A, 0x00);
	WriteByte(0xFF4B, 0x00);
	WriteByte(0xFFFF, 0x00);
}

void Memory::SetMbc(IMbc* memory)
{
	this->mbc = memory;
}

NoMbc::NoMbc(int romSize, System* system)
{
	m_romBanks = romSize / BANK_SIZE;
	m_rom.resize(romSize);
	for (auto i = 0; i < m_romBanks; i++)
	{
		m_rom[i].resize(BANK_SIZE);
	}
	m_ram.resize(0x2000);

}

uint8_t NoMbc::ReadByte(uint16_t addr)
{
	if (addr <= 0x3FFF) {
		return m_rom[0][addr];
	}
	else if (addr <= 0x7FFF) {
		return m_rom[1][addr - 0x4000];
	}
	else if (addr <= 0xBFFF) {
		return m_ram[addr - 0xA000];
	}
	throw std::exception("Invalid read");
}

void NoMbc::WriteByte(uint16_t addr, uint8_t value)
{
	if (addr >= 0xA000 && addr <= 0xBFFF) {
		m_ram[addr - 0xA000] = value;
	}
}

void NoMbc::LoadRom(std::vector<uint8_t> rom)
{
	for (int i = 0, k = 0; i < m_romBanks; i++) {
		for (int j = 0; j < BANK_SIZE; j++, k++) {
			this->m_rom[i][j] = rom[k];
		}
	}
}

Mbc1Memory::Mbc1Memory(int romSize, System* system)
{
	m_selectedRomBank = 1;
	m_selectedRamBank = 0;
	m_ramMode = false;
	m_romBanks = romSize / BANK_SIZE;

	m_rom.resize(m_romBanks);
	for (auto i = 0; i < m_romBanks;i++)
	{
		m_rom[i].resize(BANK_SIZE);
	}

	m_ram.resize(4);
	for (auto i = 0; i < 4;i++)
	{
		m_ram[i].resize(0x2000);
	}
}

uint8_t Mbc1Memory::ReadByte(uint16_t addr)
{
	if(addr <= 0x3FFF)
	{
		return m_rom[0][addr];
	}
	if(addr <= 0x7FFF)
	{
		return m_rom[m_selectedRomBank][addr - 0x4000];
	}
	if(addr <= 0xBFFF)
	{
		return m_ram[m_selectedRamBank][addr - 0xA000];
	}
	throw std::exception("Invalid address");
}

void Mbc1Memory::WriteByte(uint16_t addr, uint8_t value)
{
	if (addr <= 0x1FFF) 
	{
	}
	else if (addr <= 0x3FFF)
	{
		m_selectedRomBank = value & 0x1F;
	}
	else if(addr <= 0x5FFF)
	{
		if(m_ramMode)
		{
			m_selectedRamBank = value & 0x03;
		}
		else
		{
			m_selectedRomBank = (m_selectedRomBank & 0x1F) | ((value & 3) << 5);
		}
	}
	else if(addr <= 0x7FFF)
	{
		m_ramMode = value == 1;
	}
	else if(addr >= 0xA000 && addr <= 0xBFFF)
	{
		m_ram[m_selectedRamBank][addr - 0xA000] = value;
		// TODO: write save?
	}
}

void Mbc1Memory::LoadRom(std::vector<uint8_t> rom)
{
	for (int i = 0, k = 0; i < m_romBanks; i++) {
		for (int j = 0; j < BANK_SIZE; j++, k++) {
			m_rom[i][j] = rom[k];
		}
	}
}

Mbc3Memory::Mbc3Memory(int romSize, System* system)
{
	m_selectedRomBank = 1;
	m_selectedRamBank = 0;
	m_ramMode = false;
	m_romBanks = romSize / BANK_SIZE;
	m_ramTimerEnabled = false;
	m_rom.resize(m_romBanks);
	for (auto i = 0; i < m_romBanks; i++)
	{
		m_rom[i].resize(BANK_SIZE);
	}

	m_ram.resize(4);
	for (auto i = 0; i < 4; i++)
	{
		m_ram[i].resize(0x2000);
	}
}

uint8_t Mbc3Memory::ReadByte(uint16_t addr)
{
	if(addr >= 0 && addr <= 0x3fff)
	{
		return m_rom[0][addr];
	}
	if (addr >= 0x4000 && addr <= 0x7FFF)
	{
		return m_rom[m_selectedRomBank][addr - 0x4000];
	}
	if(addr >= 0xA000 && addr <= 0xBFFF)
	{
		if(m_ramTimerEnabled && m_selectedRamBank < 4)
		{
			return m_ram[m_selectedRamBank][addr - 0xA000];
		}
		// TODO: RTC
		return 0;
	}
	throw std::exception("Invalid memory address");
}

void Mbc3Memory::WriteByte(uint16_t addr, uint8_t value)
{
	if(addr > 0 && addr <= 0x1FFF)
	{
		m_ramTimerEnabled = value == 0x0A;
	}
	else if(addr >= 0x2000 && addr <= 0x3FFF)
	{
		m_selectedRomBank = value & 0x7F;
	}
	else if(addr >= 0x4000 && addr <= 0x5FFF)
	{
		m_selectedRamBank = value;
	}
	else if(addr >= 0x6000 && addr <= 0x7FFF)
	{
		// TODO: RTC
	}
	else if(addr >= 0xA000 && addr <= 0xBFFF)
	{
		if(m_ramTimerEnabled && m_selectedRamBank < 4)
		{
			m_ram[m_selectedRamBank][addr - 0xA000] = value;
			// TODO: write save
		}
		// TODO: RTC
	}
}

void Mbc3Memory::LoadRom(std::vector<uint8_t> rom)
{
	for (int i = 0, k = 0; i < m_romBanks; i++) {
		for (int j = 0; j < BANK_SIZE; j++, k++) {
			m_rom[i][j] = rom[k];
		}
	}
}
