#ifdef DEBUG
#include <StormLib.h>
#endif


void extractStormLib(TMPQArchive &archive, const boost::filesystem::path &filePath, const boost::filesystem::path &target)
{
	boost::filesystem::ofstream ofstream(target, std::ios::out | std::ios::binary);

	if (!ofstream)
	{
		std::cerr << boost::format(_("Couldn't open file %1%.")) % target << std::endl;

		return;
	}

	TMPQFile *file;

	if (SFileOpenFileEx(&archive, filePath.c_str(), SFILE_OPEN_FROM_MPQ, (void**)&file))
	{
		BYTE value;

		while (SFileReadFile(&file, &value, sizeof(value)))
			ofstream.write((const char*)&value, sizeof(value));

		SFileCloseFile(file);
	}
	else
		std::cerr << boost::format(_("Couldn't open file %1%.")) % filePath << std::endl;
}

#ifdef DEBUG
std::string fileInfoStormLib(TMPQFile &file, bool humanReadable, bool decimal)
{
	// boost::scoped_array leads to segmentation fault in destructor
	//DWORD *buffer = new DWORD[SFileGetFileSize(&file, 0)];
	//std::cout << "File size: " <<  SFileGetFileSize(&file, 0) << std::endl;

	/*
	SFileSetFilePointer(
	&file,                     // File handle
	0,                    // Low 32 bits of the file position
	0,             // Pointer to high 32 bits of the file position
	FILE_BEGIN                // The starting point for the file pointer move
	);
	*/

	// TODO decompression leads to segmentation fault for the second file
	BYTE value;

	while (SFileReadFile(&file, &value, sizeof(value)))
		;
		//std::cerr << boost::format(_("Warning: Couldn't read file \"%1%\" by StormLib for debugging.")) % file.pFileEntry->szFileName << std::endl;

	std::stringstream sstream;
	sstream << boost::format(_("\"%1%\"\nCompressed: %2%\nSize: %3%\nHash index: %4%\nCompressed size: %5%\nFlags: %6%\nBlock offset: %7%"))
		% file.pFileEntry->szFileName
		% boolString(file.pFileEntry->dwFlags & MPQ_FILE_COMPRESSED)
		% sizeString(SFileGetFileSize(&file, 0), humanReadable, decimal)
		% file.pFileEntry->dwHashIndex
		% sizeString(file.pFileEntry->dwCmpSize, humanReadable, decimal)
		% flagsString(static_cast<Block::Flags>(file.pFileEntry->dwFlags))
		% file.pFileEntry->ByteOffset
	;

	uint32 fileKey;
	if (SFileGetFileInfo(
		&file,                // Handle to a file or archive
		SFILE_INFO_KEY,                 // Type of information to retrieve
		&fileKey,                // Pointer to the buffer where to store the result information
		sizeof(fileKey),                 // Size of the buffer pointed by pvFileInfo
		0           // Size, in bytes, required to store information to pvFileInfo
		))
		sstream << boost::format(_("\nKey: %1%")) % fileKey;

	uint32 fileKeyUnfixed;
	if (SFileGetFileInfo(
		&file,
		SFILE_INFO_KEY_UNFIXED,
		&fileKeyUnfixed,
		sizeof(fileKeyUnfixed),
			     0))
			     sstream << boost::format(_("\nKey (without encryption): %1%")) % fileKeyUnfixed;

	//sstream << boost::format(_("%1%\nCompressed: %2%\nEncrypted: %3%\nImploded: %4%\nFlags: %5%\nCompressed size: %6%\nSize: %7%\nHash A: %8%\nHash B: %9%\nKey: %10%\nBlock index: %11%\nHash index: %12%\nHas offset table: %13%")) % file.path() % boolString(file.) % boolString(file.isEncrypted()) % boolString(file.isImploded()) % flagsString(file.block()->flags()) % sizeString(file.compressedSize(), humanReadable, decimal) % sizeString(file.size(), humanReadable, decimal) % file.hash()->hashData().filePathHashA() % file.hash()->hashData().filePathHashB() % (file.path().empty() ? 0 : file.fileKey()) % file.block()->index() % file.hash()->index() % boolString(file.hasSectorOffsetTable());

	if (file.dwSectorCount > 0)
	{
		sstream << std::endl << boost::format(_("\n%1% Sectors:")) % file.dwSectorCount;

		if (file.pFileEntry->dwFlags & MPQ_FILE_COMPRESSED)
		{
			for (DWORD i = 0; i < file.dwSectorCount; ++i)
				sstream << boost::format(_("\nSector %1%:\n-- Offset: %2%")) % i % file.SectorOffsets[i];
		}
	}

	return sstream.str();
}

void fileCheckStormLib(const mpq::MpqFile &mpqFile, TMPQFile &file, bool humanReadable, bool decimal)
{
	// TODO boost::scoped_array leads to segmentation fault in destructor
	//DWORD *buffer = new DWORD[SFileGetFileSize(&file, 0)];
	//std::cout << "File size: " <<  SFileGetFileSize(&file, 0) << std::endl;

	// TODO decompression leads to segmentation fault for the second file
	//if (!SFileReadFile(&file, &buffer, SFileGetFileSize(&file, 0)))
		//std::cerr << boost::format(_("Warning: Couldn't read file \"%1%\" by StormLib for debugging.")) % file.pFileEntry->szFileName << std::endl;

	if (file.pFileEntry->dwCmpSize != mpqFile.block()->blockSize())
	{
		throw Exception("block size");
	}

	if (file.pFileEntry->dwFileSize != mpqFile.size())
	{
		throw Exception("file size");
	}

	if (file.pFileEntry->ByteOffset != mpqFile.block()->largeOffset())
	{
		throw Exception("byte offset");
	}

	if (file.pFileEntry->wPlatform != static_cast<uint16>(mpqFile.platform()))
	{
		throw Exception("platform");
	}

	if (file.pFileEntry->lcLocale != static_cast<uint16>(mpqFile.locale()))
	{
		throw Exception("locale");
	}

	if (strcmp(file.pFileEntry->szFileName, mpqFile.path().c_str()) != 0)
	{
		throw Exception("path");
	}

	/*
	 * FIXME
	if (strcmp(GetPlainFileNameA(file.pFileEntry->szFileName), mpqFile.name().c_str()) != 0)
	{
		throw Exception("name");
	}
	*/

	if (file.pFileEntry->dwFlags != static_cast<uint16>(mpqFile.block()->flags()))
	{
		throw Exception("flags");
	}

	// NOTE check before key, more precise
	if (mpqFile.isEncrypted())
	{
		uint32 fileKeyUnfixed;
		if (SFileGetFileInfo(
			&file,
			SFILE_INFO_KEY_UNFIXED,
			&fileKeyUnfixed,
			sizeof(fileKeyUnfixed),
				0) &&
				fileKeyUnfixed != HashString(Mpq::cryptTable(), mpqFile.name().c_str(), HashType::FileKey))
			throw Exception("key without encryption");

		uint32 fileKey;
		if (SFileGetFileInfo(
			&file,                // Handle to a file or archive
			SFILE_INFO_KEY,                 // Type of information to retrieve
			&fileKey,                // Pointer to the buffer where to store the result information
			sizeof(fileKey),                 // Size of the buffer pointed by pvFileInfo
			0           // Size, in bytes, required to store information to pvFileInfo
			) && fileKey != mpqFile.fileKey())
			throw Exception("key");
	}

	MpqFile::Sectors realSectors = mpqFile.realSectors();

	if (file.dwSectorCount != realSectors.size())
		throw Exception("sectors");

	if (file.dwSectorCount > 0)
	{
		if (file.pFileEntry->dwFlags & MPQ_FILE_COMPRESSED)
		{
			for (DWORD i = 0; i < file.dwSectorCount; ++i)
			{
				if (file.SectorOffsets[i] != realSectors[i].sectorOffset())
					throw Exception(boost::format("sector offset %1%") % i);
			}
		}
	}
}

#define STORM_BUFFER_SIZE       0x500

bool compareCryptTables()
{
	DWORD StormBuffer[STORM_BUFFER_SIZE];
	DWORD dwSeed = 0x00100001;
	DWORD index1 = 0;
	DWORD index2 = 0;
	int   i;

	// Initialize the decryption buffer.
	// Do nothing if already done.
	for(index1 = 0; index1 < 0x100; index1++)
	{
		for(index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
		{
			DWORD temp1, temp2;

			dwSeed = (dwSeed * 125 + 3) % 0x2AAAAB;
			temp1  = (dwSeed & 0xFFFF) << 0x10;

			dwSeed = (dwSeed * 125 + 3) % 0x2AAAAB;
			temp2  = (dwSeed & 0xFFFF);

			StormBuffer[index2] = (temp1 | temp2);
		}
	}

	return memcmp(Mpq::cryptTable(), StormBuffer, STORM_BUFFER_SIZE) == 0;
}
#endif

#ifdef DEBUG
	if (vm.count("debug"))
	{
		ofstream infoOut(entryPath.string() + "info", std::ios::out);
		infoOut << fileInfo(*file, vm.count("human-readable"), vm.count("decimal"));

		// StormLib output

		TMPQArchive *archive;

		if (SFileOpenArchive(mpq.path().c_str(), 0, MPQ_OPEN_READ_ONLY, (void**)&archive))
		{
			TMPQFile *sFile;

			if (SFileOpenFileEx(archive, oldEntry.c_str(), SFILE_OPEN_FROM_MPQ, (void**)&sFile))
			{
				try
				{
					fileCheckStormLib(*file, *sFile, vm.count("human-readable"), vm.count("decimal"));
				}
				catch (Exception e)
				{
					std::cerr << file->path() << " - StormLib test failed with: " << e.what() << std::endl;
				}

				ofstream infoOut(entryPath.string() + "infoStormLib", std::ios::out);
				infoOut << fileInfoStormLib(*sFile, vm.count("human-readable"), vm.count("decimal"));

				SFileCloseFile(sFile);
			}
			else
				std::cerr << boost::format(_("Warning: Couldn't open file \"%1%\" by StormLib for debugging.")) % oldEntry << std::endl;

			SFileCloseArchive(archive);
		}
		else
			std::cerr << boost::format(_("Warning: Couldn't open archive %1% by StormLib for debugging.")) % mpq.path() << std::endl;
	}
#endif

#ifdef DEBUG
	if (!compareCryptTables())
		std::cerr << _("Crypt tables from wc3lib and StormLib are not equal.") << std::endl;
	else
		std::cout << _("Crypt tables from wc3lib and StormLib are equal.") << std::endl;
#endif