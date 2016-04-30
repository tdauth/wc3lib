/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
 *   tamino@cdauth.eu                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "test.hpp"
#include "attributes.hpp"

namespace wc3lib
{

namespace mpq
{


std::string flagsString(Block::Flags flags)
{
	if (flags == Block::Flags::None)
	{
		return _("Uncompressed");
	}
	else
	{
		bool empty = true;
		stringstream sstream;

		if (flags & Block::Flags::IsFile)
		{
			sstream << _("File");
			empty = false;
		}

		if (flags & Block::Flags::IsSingleUnit)
		{
			if (!empty)
			{
				sstream << ", ";
			}

			sstream << _("Single Unit");
			empty = false;
		}

		if (flags & Block::Flags::UsesEncryptionKey)
		{
			if (!empty)
			{
				sstream << ", ";
			}

			sstream << _("Uses Encryption Key");
			empty = false;
		}

		if (flags & Block::Flags::IsEncrypted)
		{
			if (!empty)
			{
				sstream << ", ";
			}

			sstream << _("Is Encrypted");
			empty = false;
		}

		if (flags & Block::Flags::IsCompressed)
		{
			if (!empty)
			{
				sstream << ", ";
			}

			sstream << _("Is Compressed");
			empty = false;
		}

		if (flags & Block::Flags::IsImploded)
		{
			if (!empty)
			{
				sstream << ", ";
			}

			sstream << _("Is Imploded");
			empty = false;
		}

		return sstream.str();
	}

	return "";
}

std::string compressionString(Sector::Compression compression)
{
	if (compression == Sector::Compression::Uncompressed)
	{
		return _("Uncompressed");
	}
	else
	{
		bool empty = true;
		stringstream sstream;

		if (compression & Sector::Compression::ImaAdpcmMono)
		{
			sstream << _("ImaAdpcmMono");
			empty = false;
		}

		if (compression & Sector::Compression::ImaAdpcmStereo)
		{
			if (!empty)
			{
				sstream << ", ";
			}

			sstream << _("ImaAdpcmStereo");
			empty = false;
		}

		if (compression & Sector::Compression::Huffman)
		{
			if (!empty)
			{
				sstream << ", ";
			}

			sstream << _("Huffman");
			empty = false;
		}

		if (compression & Sector::Compression::Deflated)
		{
			if (!empty)
			{
				sstream << ", ";
			}

			sstream << _("Deflated");
			empty = false;
		}

		if (compression & Sector::Compression::Imploded)
		{
			if (!empty)
			{
				sstream << ", ";
			}

			sstream << _("Imploded");
			empty = false;
		}

		if (compression & Sector::Compression::Bzip2Compressed)
		{
			if (!empty)
			{
				sstream << ", ";
			}

			sstream << _("Bzip2Compressed");
			empty = false;
		}

		if (compression & Sector::Compression::Sparse)
		{
			if (!empty)
			{
				sstream << ", ";
			}

			sstream << _("Sparse");
			empty = false;
		}

		if (compression & Sector::Compression::Lzma)
		{
			if (!empty)
			{
				sstream << ", ";
			}

			sstream << _("Lzma");
			empty = false;
		}

		return sstream.str();
	}

	return "";
}

std::string fileInfo(File &file, bool humanReadable, bool decimal)
{
	Sector::Sectors sectors;
	file.sectors(sectors);
	std::stringstream sstream;
	sstream << boost::format(_("%1%\nCompressed: %2%\nEncrypted: %3%\nImploded: %4%\nFlags: %5%\nCompressed size: %6%\nSize: %7%\nHash A: %8%\nHash B: %9%\nKey: %10%\nBlock index: %11%\nHash index: %12%\nHas offset table: %13%\nBlock offset: %14%\nKey (without encryption): %15%"))
	% file.path() // 1
	% boolString(file.isCompressed()) // 2
	% boolString(file.isEncrypted()) // 3
	% boolString(file.isImploded()) // 4
	% flagsString(file.block()->flags()) // 5
	% sizeString(file.compressedSize(), humanReadable, decimal) // 6
	% sizeString(file.size(), humanReadable, decimal) // 7
	% file.hash()->cHashData().filePathHashA() // 8
	% file.hash()->cHashData().filePathHashB() // 9
	% (file.name().empty() || !file.isEncrypted() ? 0 : file.fileKey()) // 10
	% file.block()->index() // 11
	% file.hash()->index() // 12
	% boolString(file.hasSectorOffsetTable()) // 13
	% file.block()->largeOffset() // 14
	% (file.isEncrypted() ? HashString(Archive::cryptTable(), file.name().c_str(), HashType::FileKey) : 0) // 15
	;

	if (sectors.size() > 0)
	{
		sstream << std::endl << boost::format(_("\n%1% Sectors:")) % sectors.size();

		BOOST_FOREACH(Sector::Sectors::const_reference sector, sectors)
		{
				sstream << boost::format(_("\nSector %1%:\n-- Offset: %2%\n-- Size: %3%\n-- Compression: %4%"))
					% sector.sectorIndex()
					% sector.sectorOffset()
					% sizeString(sector.sectorSize(), humanReadable, decimal)
					% compressionString(sector.compression())
				;
		}
	}

	return sstream.str();
}

std::string formatString(Archive::Format format)
{
	switch (format)
	{
		case Archive::Format::Mpq1:
			return _("MPQ1");

		case Archive::Format::Mpq2:
			return _("MPQ2");
	}

	return _("Invalid");
}

std::string archiveInfo(Archive &archive, bool humanReadable, bool decimal)
{
	std::stringstream sstream;
	sstream << boost::format(_("%1%\nSize: %2%\nHashes: %3%\nBlocks: %4%\nFormat %5%\nSector size: %6%\nHas (listfile) file: %7%\nHas (attributes) file: %8%\nHas (signature) file: %9%\nHas strong digital signature: %10%\n"))
		% archive.path() // 1
		% sizeString(archive.size(), humanReadable, decimal) // 2
		% archive.hashes().size() // 3
		% archive.blocks().size() // 4
		% formatString(archive.format()) // 5
		% archive.sectorSize() // 6
		% boolString(archive.containsListfileFile()) // 7
		% boolString(archive.containsAttributesFile()) // 8
		% boolString(archive.containsSignatureFile()) // 9
		% boolString(archive.hasStrongDigitalSignature()) // 10
	;

	if (archive.containsAttributesFile())
	{
		Attributes attributes = archive.attributesFile();
		int32 version = 0;
		Attributes::ExtendedAttributes extendedAttributes = Attributes::ExtendedAttributes::None;
		Attributes::Crc32s crcs;
		Attributes::FileTimes fileTimes;
		Attributes::Md5s md5s;
		attributes.attributes(version, extendedAttributes, crcs, fileTimes, md5s);

		if (extendedAttributes != Attributes::ExtendedAttributes::None)
		{
			sstream
			<< boost::format(_("Extended attributes:\nHas CRC32s: %1%\nHas time stamps: %2%\nHas MD5s: %3%\nNumber of CRC32s: %4%\nNumber of time stamps: %5%\nNumber of MD5s: %6%"))
			% boolString(extendedAttributes & Attributes::ExtendedAttributes::FileCrc32s)
			% boolString(extendedAttributes & Attributes::ExtendedAttributes::FileTimeStamps)
			% boolString(extendedAttributes & Attributes::ExtendedAttributes::FileMd5s)
			% crcs.size()
			% fileTimes.size()
			% md5s.size()
			;
		}
	}

	return sstream.str();
}

}

}
