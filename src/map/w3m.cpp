/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#include "w3m.hpp"

namespace wc3lib
{

namespace map
{

W3m::W3m() :
m_environment(new Environment())
, m_shadow(new Shadow(0, 0))
, m_pathmap(new Pathmap())
, m_trees(new Trees())
, m_customUnits(new CustomUnits())
, m_info(new Info())
, m_strings(new MapStrings())
, m_minimap(new Minimap())
, m_menuMinimap(new MenuMinimap())
, m_triggers(new Triggers())
, m_cameras(new Cameras())
, m_rects(new Rects())
, m_sounds(new Sounds())
, m_customTextTriggers(new CustomTextTriggers())
, m_importedFiles(new ImportedFiles())
{
}

W3m::~W3m()
{
}

#ifdef MPQ
std::streamsize W3m::readFileFormat(FileFormat *format)
{
	if (format == 0)
	{
		return 0;
	}

	std::streamsize size = 0;
	mpq::File file = this->findFile(format->fileName());

	if (file.isValid())
	{
		/*
		 * FIXME
		boost::scoped_array<byte> buffer(new byte[file->size()]);
		arraystream stream(buffer.get(), file->size());
		*/
		stringstream stream;

		try
		{
			file.writeData(stream);
			size += format->read(stream);
		}
		catch (std::exception &exception)
		{
			throw Exception(boost::format(_("Error while reading map file \"%1%\":\n%2%")) % format->fileName() % exception.what());
		}
	}

	return size;
}

std::streamsize W3m::readAllFileFormats(const TriggerData *triggerData)
{
	std::streamsize size = 0;

	size += this->readFileFormat(this->environment().get());
	// NOTE always read environment before shadow to get width and height of map!
	size += this->readFileFormat(this->shadow().get());

	size += this->readFileFormat(this->pathmap().get());
	size += this->readFileFormat(this->trees().get());
	size += this->readFileFormat(this->customUnits().get());
	size += this->readFileFormat(this->info().get());
	size += this->readFileFormat(this->strings().get());
	size += this->readFileFormat(this->minimap().get());
	size += this->readFileFormat(this->menuMinimap().get());
	size += this->readFileFormat(this->cameras().get());
	size += this->readFileFormat(this->rects().get());
	size += this->readFileFormat(this->sounds().get());
	size += this->readFileFormat(this->customTextTriggers().get());
	size += this->readFileFormat(this->importedFiles().get());

	// NOTE do not read triggers without trigger data. Triggers have to be read separately!
	if (triggerData != 0)
	{
		size += readTriggers(*triggerData);
	}

	return size;
}


std::streamsize W3m::read(InputStream &istream)
{
	std::streamsize size = this->readHeader(istream);
	size += mpq::Archive::read(istream);
	size += this->readSignature(istream);

	return size;
}

std::streamsize W3m::readTriggers(const TriggerData &triggerData)
{
	std::streamsize size = 0;
	mpq::File file = this->findFile(m_triggers.get()->fileName());

	if (file.isValid())
	{
		/*
		 * FIXME
		boost::scoped_array<byte> buffer(new byte[file->size()]);
		arraystream stream(buffer.get(), file->size());
		*/
		stringstream stream;

		try
		{
			file.writeData(stream);
			size += m_triggers.get()->read(stream, triggerData);
		}
		catch (std::exception &exception)
		{
			throw Exception(boost::format(_("Error while reading map file \"%1%\":\n%2%")) % m_triggers.get()->fileName() % exception.what());
		}
	}
	else
	{
		throw Exception(boost::format(_("Couldn't find \"%1%\"")) % m_triggers.get()->fileName());
	}

	return size;
}

#endif

std::streamsize W3m::write(OutputStream &ostream) const
{
	std::streamsize size = this->writeHeader(ostream);
	// TODO create new MPQ archive in stream ostream with all corresponding file formats as MpqFile instances
	//size += mpq::Mpq::write(ostream);

	return size;
}

std::streamsize W3m::readHeader(InputStream &istream)
{
	std::streamsize size = 0;
	int32 fileId;
	wc3lib::read(istream, fileId, size);

	if (fileId != this->fileId())
	{
		throw Exception(boost::format(_("W3m: Unknown file id \"%1%\". Expected \"%2%\".")) % fileId % this->fileId());
	}

	int32 unknown;
	wc3lib::read(istream, unknown, size);
	wc3lib::readString(istream, m_name, size);
	wc3lib::read<int32>(istream, (int32&)m_flags, size);
	wc3lib::read(istream, m_maxPlayers, size);
	std::size_t byteCount = 512 - size; // followed by 00 bytes until the 512 bytes of the header are filled.
	istream.ignore(byteCount);

	return size;
}

std::streamsize W3m::readSignature(InputStream &istream)
{
	std::streamsize result = 0;

	/*
	 * Get the end position to detect if any stream data is still available.
	 */
	const std::streampos end = endPosition(istream);

	for (std::streampos pos = istream.tellg(); pos < (std::streamoff)end - 4; pos = istream.tellg())
	{
		byte signId[4];
		wc3lib::read(istream, signId, result, 4);

		// footer is optional!
		if (memcmp(signId, "NGIS", sizeof(signId)) == 0)
		{
			m_signature.reset(new byte[signatureSize]);
			wc3lib::read(istream, m_signature[0], result, signatureSize);

			break;
		}
	}

	return result;
}

std::streamsize W3m::writeHeader(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, fileId(), size);
	wc3lib::write<int32>(ostream, 0, size); // unknown
	wc3lib::writeString(ostream, name(), size);
	wc3lib::write<int32>(ostream, (int32)flags(), size);
	wc3lib::write(ostream, maxPlayers(), size);
	std::size_t byteCount = 512 - size; // followed by 00 bytes until the 512 bytes of the header are filled.
	ostream.seekp(byteCount, std::ios::cur);

	return size;
}

}

}
