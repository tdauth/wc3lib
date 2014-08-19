#ifndef WC3LIB_MPQ_TEST
#define WC3LIB_MPQ_TEST

#include "mpq.hpp"

/**
 * \file
 * Defines test functions for debugging the MPQ format support.
 * The functions bring properties of archives, files and sectors in a readable form which could be printed
 * to the output stream of written into a file.
 */

namespace wc3lib
{

namespace mpq
{

template<typename T>
std::string sizeString(T size, bool humanReadable, bool decimal)
{
	if (humanReadable)
	{
		if (decimal)
			return sizeStringDecimal<T>(size);
		else
			return sizeStringBinary<T>(size);
	}

	std::ostringstream result;
	result << size;

	return result.str();
}

std::string flagsString(Block::Flags flags);
std::string compressionString(Sector::Compression compression);
std::string fileInfo(MpqFile &file, bool humanReadable, bool decimal);
std::string formatString(Mpq::Format format);
std::string archiveInfo(Mpq &archive, bool humanReadable, bool decimal);

}

}


#endif
