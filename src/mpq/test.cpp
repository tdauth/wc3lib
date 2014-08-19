#include "test.hpp"

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

std::string fileInfo(MpqFile &file, bool humanReadable, bool decimal)
{
	MpqFile::Sectors sectors;
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
	% (file.isEncrypted() ? HashString(Mpq::cryptTable(), file.name().c_str(), HashType::FileKey) : 0) // 15
	;

	if (sectors.size() > 0)
	{
		sstream << std::endl << boost::format(_("\n%1% Sectors:")) % sectors.size();

		BOOST_FOREACH(MpqFile::Sectors::const_reference sector, sectors)
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

std::string formatString(Mpq::Format format)
{
	switch (format)
	{
		case Mpq::Format::Mpq1:
			return _("MPQ1");

		case Mpq::Format::Mpq2:
			return _("MPQ2");
	}

	return _("Invalid");
}

std::string archiveInfo(Mpq &archive, bool humanReadable, bool decimal)
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

		if (attributes.extendedAttributes() != Attributes::ExtendedAttributes::None)
		{
			sstream << boost::format(_("Extended attributes:\nHas CRC32s: %1%\nHas time stamps: %2%\nHas MD5s: %3%")) % boolString(attributes.extendedAttributes() & Attributes::ExtendedAttributes::FileCrc32s) % boolString(attributes.extendedAttributes() & Attributes::ExtendedAttributes::FileTimeStamps) % boolString(attributes.extendedAttributes() & Attributes::ExtendedAttributes::FileMd5s);
		}
	}

	return sstream.str();
}

}

}
