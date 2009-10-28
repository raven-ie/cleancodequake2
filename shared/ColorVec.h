/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

// ColorVec.h

/*
==============================================================================

	colorf
 
==============================================================================
*/

class colorf
{
public:
	/**
	 * Data
	 */
	float R, G, B, A;

	/**
	 * Constructors
	 */
	colorf() 
		: R(0), G(0), B(0), A(0){}

	inline colorf(const colorf &Other)
		: R(Other.R), G(Other.G), B(Other.B), A(Other.A) {}

	inline colorf(const class colorb &Other);

	inline colorf(const float InR, const float InG, const float InB, const float InA)
		: R(InR), G(InG), B(InB), A(InA) {}

	/**
	 * Destructors
	 */
	~colorf() {}

	/**
	 * Operators
	 */
	inline bool operator !=(const colorf &Other) { return (R != Other.R || G != Other.G || B != Other.B || A != Other.A); }
	inline const bool operator !=(const colorf &Other) const { return (R != Other.R || G != Other.G || B != Other.B || A != Other.A); }

	inline bool operator ==(const colorf &Other) { return (R == Other.R && G == Other.G && B == Other.B && A == Other.A); }
	inline const bool operator ==(const colorf &Other) const { return (R == Other.R && G == Other.G && B == Other.B && A == Other.A); }

	inline const float &operator [](const int Index) const { return (&R)[Index]; }
	inline float &operator [](const int Index) { return (&R)[Index]; }

	inline operator float *() { return (&R); }
	inline operator const float *() const { return (&R); }

	inline colorf &operator =(const colorf &Other)
	{
		R = Other.R;
		G = Other.G;
		B = Other.B;
		A = Other.A;

		return *this;
	}

	/**
	 * Functions
	 */
	inline void Set(const colorf &Other) { R = Other.R; G = Other.G; B = Other.B; A = Other.A; }
	inline void Set(const float InR, const float InG, const float InB, const float InA) { R = InR; G = InG; B = InB; A = InA; }
};

/*
==============================================================================

	colorb
 
==============================================================================
*/

class colorb
{
public:
	/**
	 * Data
	 */
	byte R, G, B, A;

	/**
	 * Constructors
	 */
	colorb() 
		: R(0), G(0), B(0), A(0) {}

	inline colorb(const colorb &Other)
		: R(Other.R), G(Other.G), B(Other.B), A(Other.A) {}

	inline colorb(const byte InR, const byte InG, const byte InB, const byte InA)
		: R(InR), G(InG), B(InB), A(InA) {}

	inline colorb(const colorf &Other)
		: R(FloatToByte(Other.R))
		, G(FloatToByte(Other.G))
		, B(FloatToByte(Other.B))
		, A(FloatToByte(Other.A)) {}

#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
	inline colorb(const vec4_t Other)
		: R(FloatToByte(Other[0]))
		, G(FloatToByte(Other[1]))
		, B(FloatToByte(Other[2]))
		, A(FloatToByte(Other[3])) {}
#endif

	/**
	 * Destructors
	 */
	~colorb() {}

	/**
	 * Operators
	 */
	inline bool operator !=(const colorb &Other) { return (R != Other.R || G != Other.G || B != Other.B || A != Other.A); }
	inline const bool operator !=(const colorb &Other) const { return (R != Other.R || G != Other.G || B != Other.B || A != Other.A); }

	inline bool operator ==(const colorb &Other) { return (R == Other.R && G == Other.G && B == Other.B && A == Other.A); }
	inline const bool operator ==(const colorb &Other) const { return (R == Other.R && G == Other.G && B == Other.B && A == Other.A); }

	inline const byte &operator [](const int Index) const { return (&R)[Index]; }
	inline byte &operator [](const int Index) { return (&R)[Index]; }

	inline operator byte *() { return (&R); }
	inline operator const byte *() const { return (&R); }

	inline colorb &operator =(const colorb &Other)
	{
		*(int *)&R = *(int *)&Other.R;
		return *this;
	}

	// GLbyte isn't unsigned
	inline operator signed char *() { return (signed char*)(&R); }
	inline operator const signed char *() const { return (signed char*)(&R); }

	/**
	 * Functions
	 */
	inline void Set(const colorb &Other) { R = Other.R; G = Other.G; B = Other.B; A = Other.A; }
	inline void Set(const byte InR, const byte InG, const byte InB, const byte InA) { R = InR; G = InG; B = InB; A = InA; }

	inline void Set(const colorf &Other) { R = FloatToByte(Other.R); G = FloatToByte(Other.G); B = FloatToByte(Other.B); A = FloatToByte(Other.A); }
};

inline colorf::colorf(const colorb &Other)
	: R(Other.R / 255), G(Other.G / 255), B(Other.B / 255), A(Other.A / 255) {}

/*
==============================================================================

	COLOR STRING HANDLING
 
==============================================================================
*/

#define NUM_COLOR_TABLE_COLORS 9

extern const colorf	Q_FColorBlack;
extern const colorf	Q_FColorRed;
extern const colorf	Q_FColorGreen;
extern const colorf	Q_FColorYellow;
extern const colorf	Q_FColorBlue;
extern const colorf	Q_FColorCyan;
extern const colorf	Q_FColorMagenta;
extern const colorf	Q_FColorWhite;

extern const colorf	Q_FColorLtGrey;
extern const colorf	Q_FColorMdGrey;
extern const colorf	Q_FColorDkGrey;

extern const colorf	Q_FStrColorTable[NUM_COLOR_TABLE_COLORS];

extern const colorb	Q_BColorBlack;
extern const colorb	Q_BColorRed;
extern const colorb	Q_BColorGreen;
extern const colorb	Q_BColorYellow;
extern const colorb	Q_BColorBlue;
extern const colorb	Q_BColorCyan;
extern const colorb	Q_BColorMagenta;
extern const colorb	Q_BColorWhite;

extern const colorb	Q_BColorLtGrey;
extern const colorb	Q_BColorMdGrey;
extern const colorb	Q_BColorDkGrey;

extern const colorb	Q_BStrColorTable[NUM_COLOR_TABLE_COLORS];

#define COLOR_ESCAPE	'^'

#define COLOR_BLACK		'0'
#define COLOR_RED		'1'
#define COLOR_GREEN		'2'
#define COLOR_YELLOW	'3'
#define COLOR_BLUE		'4'
#define COLOR_CYAN		'5'
#define COLOR_MAGENTA	'6'
#define COLOR_WHITE		'7'
#define COLOR_GREY		'8'

#define STYLE_ITALIC	'I'
#define STYLE_RETURN	'R'
#define STYLE_SHADOW	'S'

#define S_COLOR_ESCAPE	"^"

#define S_COLOR_BLACK	"^0"
#define S_COLOR_RED		"^1"
#define S_COLOR_GREEN	"^2"
#define S_COLOR_YELLOW	"^3"
#define S_COLOR_BLUE	"^4"
#define S_COLOR_CYAN	"^5"
#define S_COLOR_MAGENTA	"^6"
#define S_COLOR_WHITE	"^7"
#define S_COLOR_GREY	"^8"

#define S_STYLE_ITALIC	"^I"
#define S_STYLE_RETURN	"^R"
#define S_STYLE_SHADOW	"^S"

inline int Q_StrColorIndex (char c)
{
	return (((c & 127) - '0') % NUM_COLOR_TABLE_COLORS);
}

bool		Q_IsColorString (const char *p);
int			Q_ColorCharCount (const char *s, int endPos);
int			Q_ColorCharOffset (const char *s, int charCount);
int			Q_ColorStrLastColor (char *s, int byteOfs);
int			Q_ColorStrLastStyle (char *s, int byteOfs);

inline byte HexColor_GetR (int rgba)
{
	return (rgba & 0xFF);
}

inline byte HexColor_GetG (int rgba)
{
	return ((rgba >> 8) & 0xFF);
}

inline byte HexColor_GetB (int rgba)
{
	return ((rgba >> 16) & 0xFF);
}

inline byte HexColor_GetA (int rgba)
{
	return ((rgba >> 24) & 0xFF);
}

inline int Color_RGBToHex (byte r, byte g, byte b)
{
	return ((r << 0) | (g << 8) | (b << 16));
}

inline int Color_RGBAToHex (byte r, byte g, byte b, byte a)
{
	return (r | (g << 8) | (b << 16) | (a << 24));
}
