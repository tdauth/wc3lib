#ifndef WC3LIB_PLATFORM_HPP
#define WC3LIB_PLATFORM_HPP

#include <istream>
#include <ostream>
#include <sstream>

#include <boost/cstdint.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/array.hpp>

#include "config.h"

namespace wc3lib
{

typedef char byte;
typedef int8_t int8; // not unsigned
typedef uint8_t uint8;
/**
* Short Integers
* Short Integers are stored using 2 bytes in "Little Endian" order.
* They are close to the C++ signed short but their range is from -16384 to 16383. It means the 2 highest bit are free of use for a flag for example.
* Size: 2 bytes
*/
typedef int16_t int16;
typedef uint16_t uint16;
/**
* Integers
* Intergers are stored using 4 bytes in "Little Endian" order. It means that the first byte read is the lowest byte.
* They are just like the C++ "int" (signed) type. In some other documentation of this kind you may see them named "long".
* Size: 4 bytes
* Example: 1234 decimal = [00 00 04 D2]h will be stored in this order: [D2 04 00 00]h
*/
typedef int32_t int32;
typedef uint32_t uint32; // used for various encryption and hash functions
typedef int64_t int64;
typedef uint64_t uint64; // used for large offsets

/**
* Floats
* Floats are using the standard IEEE 32bit float format. They are stored using 4 bytes and the "Little Endian" order.
* They are just like the C++ "float" type.
* Size: 4 bytes
* Example: 7654.32 decimal, this number can't be stored using this format so the system will take the closest value that can be represented using binary digits. The closest one is: 7654.319824 decimal = [45 EF 32 8F]h and will be stored as [8F 32 EF 45]h
*
* MDL notes:
* Floating point numbers are in IEEE scientific notation format with 6 significant
* figures. Exponent is not shown for exponents of ±4. If the number is an integral
* float, decimal point is not shown.
*/
typedef float float32;

typedef std::basic_istream<byte> istream;
typedef std::basic_ostream<byte> ostream;
/**
* Trigger Strings and Strings
*
* Strings are just arrays of chars terminated with a null char (C++ '\0'). However Blizzard sometimes use special control codes to change the displayed color for the string. These codes are like "|cAABBGGRR" where "AA", "RR", "GG" and "BB" are hexadecimal values (using 2 digits each) for the alpha, red, the green and the blue values. If a string starts with "TRIGSTR_" (case sensitive), it's considered as a trigger string. A trigger string is kept in memory as is ("TRIGSTR_***") and is only changed when Warcraft 3 needs to display it. Instead of just writing "TRIGSTR_000" on the user screen, War3 will look in its trigger string table created when the map was loaded and display the corresponding trigger string instead. Trigger strings only work for files inside a w3m (Jass, w3i, ...) except for the WTS which is used to define the trigger string table itself. If the number following "TRIGSTR_" is negative the trigger string will refer to a null (empty) string, if "TRIGSTR_" is followed by text, it'll be
considered as trigger string #0 ( = "TRIGSTR_000").
* "TRIGSTR_7", "TRIGSTR_07", "TRIGSTR_007" and "TRIGSTR_7abc" are all representing trigger string #7. "TRIGSTR_ab7", "TRIGSTR_abc" and "TRIGSTR_" refer to trigger string #0. "TRIGSTR_-7" is negative and will not refer to a trigger string; it'll be displayed as "". By convention, "TRIGSTR_" is followed by 3 digits and the null char that ends the string.
* Example 1: your got the string "blah |c00FF8000blah", War3 will display "blah blah" but the second "bla h" will be orange (red=FF + green=80 + blue=00 ==> orange)
 The alpha value doesn't really work btw.
Example 2: you got "TRIGSTR_025" and trigger string 25 is defined (in the .wts file) as "blah|c00FF8000blah", it'll display the same result as the previous example.
 Size (string): vary. String length + 1 (null terminating char)
Size (trigger string): 12 bytes

For its strings Warcraft uses a unicode format called UTF-8. They do this because the files need to be localized into many different languages. This format uses one byte for the most common characters which is equal to the character's ASCII value. For example A = 65 or 0x41. For the more unusual characters it can take from 2 to 6 bytes per character. For example the German letter Ä would be represented by 195 and 164 or 0xC3A4. The higher the first byte is, the more bytes are required to represent the character. Simple modulo calculations are enough to convert UTF to common unicode (UCS) and back. To convert UTF to UCS use this pattern:

If FirstByte <= 191 return FirstByte
 If 192 <= FirstByte <= 223 return (FirstByte - 192) * 64 + (SecondByte - 128)
 If 224 <= FirstByte <= 239 return (FirstByte - 224) * 4096 + (SecondByte - 128) * 64 + (ThirdByte - 128)
 If 240 <= FirstByte <= 247 return (FirstByte - 240) * 262144 + (SecondByte - 128) * 4096 + (ThirdByte - 128) * 64 + (FourthByte - 128)
 If 248 <= FirstByte <= 251 return (FirstByte - 248) * 16777216 + (SecondByte - 128) * 262144 + (ThridByte - 128) * 4096 + (FourthByte - 128) * 64 + (FifthByte - 128)
 If 252 <= FirstByte return (FirstByte - 252) * 1073741824 + (SecondByte - 128) * 16777216 + (ThirdByte - 128) * 262144 + (FourthByte - 128) * 4096 + (FifthByte - 128) * 64 + (SixthByte - 128)
To convert UCS back to UTF use this:
If ASCII <= 127 FirstByte = ASCII
 If 128 <= ASCII <= 2047 FirstByte = 192 + (ASCII \ 64) SecondByte = 128 + (ASCII Mod 64)
 If 2048 <= ASCII <= 65535 FirstByte = 224 + (ASCII \ 4096) SecondByte = 128 + ((ASCII \ 64) Mod 64) ThirdByte = 128 + (ASCII Mod 64)
 If 65536 <= ASCII <= 2097151 FirstByte = 240 + (ASCII \ 262144) SecondByte = 128 + ((ASCII \ 4096) Mod 64) ThirdByte = 128 + ((ASCII \ 64) Mod 64) FourthByte = 128 + (ASCII Mod 64)
 If 2097152 <= ASCII <= 67108863 FirstByte = 248 + (ASCII \ 16777216) SecondByte = 128 + ((ASCII \ 262144) Mod 64) ThirdByte = 128 + ((ASCII \ 4096) Mod 64) FourthByte = 128 + ((ASCII \ 64) Mod 64) FifthByte = 128 + (ASCII Mod 64)
 If 67108864 <= ASCII <= 2147483647 FirstByte = 252 + (ASCII \ 1073741824) SecondByte = 128 + ((ASCII \ 16777216) Mod 64) ThirdByte = 128 + ((ASCII \ 262144) Mod 64) FourthByte = 128 + ((ASCII \ 4096) Mod 64) FifthByte = 128 + ((ASCII \ 64) Mod 64) SixthByte = 128 + (ASCII Mod 64))
The conversion will only be needed if you want to display text in your application or write user input to files. For all other purposes you can internally treat the UTF-Strings just like ordinary strings.
*/
typedef std::basic_string<byte> string;
typedef std::basic_stringstream<byte> stringstream;
typedef std::basic_istringstream<byte> istringstream;
typedef std::basic_ostringstream<byte> ostringstream;
typedef boost::filesystem::basic_fstream<byte> fstream;
typedef boost::filesystem::basic_ifstream<byte> ifstream;
typedef boost::filesystem::basic_ofstream<byte> ofstream;
typedef boost::iostreams::stream<boost::iostreams::basic_array<byte> > arraystream;
typedef boost::iostreams::stream<boost::iostreams::basic_array_source<byte> > iarraystream;
typedef boost::iostreams::stream<boost::iostreams::basic_array_sink<byte> > oarraystream;

/**
* Flags
*
* Flags are boolean values (true or false, 1 or 0). They can be stored using 4 bytes. Each bit is a flag (4 bytes = 32 bit = 32 flags). Blizzard uses integers to store its flags.
* Size: usually 4 bytes
*/
typedef uint32_t flag;

/**
* Chars and Array of Chars
* They are just stored like standard chars (1 char = 1 byte) and array of chars (no null terminating char needed).
* Size (chars): 1 byte
* Size (array of chars): usually 4 bytes
*/
typedef boost::array<byte, 4> charArray;

/*
template<byte... Bytes>
struct __CheckBytes
{
	static const bool valid = false;
};

template<byte Left>
struct __CheckBytes<Left>
{
	static const bool valid = (
		Left == '0' ||
		Left == '1' ||
		Left == '2' ||
		Left == '3' ||
		Left == '4' ||
		Left == '5' ||
		Left == '6' ||
		Left == '7' ||
		Left == '8' ||
		Left == '9' ||
		Left == 'A' ||
		Left == 'B' ||
		Left == 'C' ||
		Left == 'D' ||
		Left == 'E' ||
		Left == 'F' ||
		Left == 'G' ||
		Left == 'H' ||
		Left == 'I' ||
		Left == 'J' ||
		Left == 'K' ||
		Left == 'L' ||
		Left == 'M' ||
		Left == 'N' ||
		Left == 'O' ||
		Left == 'P' ||
		Left == 'Q' ||
		Left == 'R' ||
		Left == 'S' ||
		Left == 'T' ||
		Left == 'U' ||
		Left == 'V' ||
		Left == 'W' ||
		Left == 'X' ||
		Left == 'Y' ||
		Left == 'Z'
	);
};

template<byte Left, byte... Bytes>
struct __CheckBytes<Left, Bytes...>
{
	static const bool valid = __CheckBytes<Left>::valid
	&& __CheckBytes<Bytes...>::valid;
};

class Id
{
	public:
		constexpr inline Id(const byte bytes[4]) : m_bytes{ bytes[0], bytes[1], bytes[2], bytes[3] }
		{
		}

	private:
		const byte m_bytes[4];
};

template<byte... Bytes>
inline constexpr Id
operator"" _id() noexcept
{
	static_assert(sizeof...(Bytes) != 4, "Id has invalid size.");
	static_assert(__CheckBytes<Bytes...>::valid, "Invalid digit in id.");

	return Id((byte[]){Bytes... });
}
*/

}

#endif
