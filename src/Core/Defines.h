#ifndef DEFINES_H
#define DEFINES_H
#include <cstdint>

typedef uint8_t u8;
typedef uint16_t u16;

struct rgb {
	unsigned char r, g, b;
};

typedef struct rgb COLOUR;

struct sprite {
#ifndef BIG_E
	unsigned char y;
	unsigned char x;
	unsigned char tile;
	struct options {
		unsigned char priority : 1;
		unsigned char vFlip : 1;
		unsigned char hFlip : 1;
		unsigned char palette : 1;
		unsigned char unkown : 1;
		unsigned char unkown_2 : 1;
		unsigned char aboveBG : 1;
	} options;
#else
	unsigned char y;
	unsigned char x;
	unsigned char tile;
	struct options {
		//unsigned char dummy : 4;
		unsigned char palette : 1;
		unsigned char hFlip : 1;
		unsigned char vFlip : 1;
		unsigned char priority : 1;
	};
#endif
};


#define LITTLE_E
struct keys1 {
#ifdef LITTLE_E
	unsigned char a : 1;
	unsigned char b : 1;
	unsigned char select : 1;
	unsigned char start : 1;
#else
	unsigned char start : 1;
	unsigned char select : 1;
	unsigned char b : 1;
	unsigned char a : 1;
#endif
};

struct keys2 {
#ifdef LITTLE_E
	unsigned char right : 1;
	unsigned char left : 1;
	unsigned char up : 1;
	unsigned char down : 1;
#else
	unsigned char down : 1;
	unsigned char up : 1;
	unsigned char left : 1;
	unsigned char right : 1;
#endif
};

struct keys {
	union {
		struct {
			union {
				keys1 keys1;
				unsigned char keys1Value : 4;
			};

			union {
				keys2 keys2;
				unsigned char keys2Value : 4;
			};
		};

		unsigned char c;
	};
};


inline bool IsBitSet(const u8 value, const int bit)
{
	return (value & (0x01 << bit)) != 0;
}

inline u8 SetBit(const u8 value, const u8 bit)
{
	return value | (0x01 << bit);
}

#endif
