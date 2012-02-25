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

W3m::W3m() : m_environment(new Environment(this)), m_shadow(new Shadow(this)), m_pathmap(new Pathmap(this)),
m_trees(new Trees()),
m_customUnits(new CustomUnits()),
m_info(new Info(this)),
m_strings(new MapStrings()),
m_minimap(new Minimap()),
m_menuMinimap(new MenuMinimap(this)),
m_triggers(new Triggers(this)),
m_cameras(new Cameras(this)),
m_rects(new Rects(this)),
m_sounds(new Sounds(this)),
m_customTextTriggers(new CustomTextTriggers()),
m_importedFiles(new ImportedFiles(this)), m_fileFormats(16)
{
	this->fileFormats()[0] = m_environment.get();
	this->fileFormats()[1] = m_shadow.get();
	this->fileFormats()[2] = m_triggers.get();
	this->fileFormats()[3] = m_pathmap.get();
	this->fileFormats()[4] = m_trees.get();
	this->fileFormats()[5] = m_customUnits.get();
	this->fileFormats()[6] = m_info.get();
	this->fileFormats()[7] = m_strings.get();
	this->fileFormats()[8] = m_minimap.get();
	this->fileFormats()[9] = m_menuMinimap.get();
	this->fileFormats()[10] = m_triggers.get();
	this->fileFormats()[11] = m_cameras.get();
	this->fileFormats()[12] = m_rects.get();
	this->fileFormats()[13] = m_sounds.get();
	this->fileFormats()[14] = m_customTextTriggers.get();
	this->fileFormats()[15] = m_importedFiles.get();
}

W3m::~W3m()
{
}

std::streamsize W3m::read(InputStream &istream, const mpq::Listfile::Entries &listfileEntries) throw (class Exception)
{
	std::streamsize size = this->readHeader(istream);
	size += mpq::Mpq::read(istream, listfileEntries);

	BOOST_FOREACH(FileFormats::reference format, fileFormats())
	{
		mpq::Mpq::FilePtr file = this->findFile(format->fileName());

		if (file.get() != 0)
		{
			/*
			 * FIXME
			boost::scoped_array<byte> buffer(new byte[file->size()]);
			arraystream stream(buffer.get(), file->size());
			*/
			stringstream stream;

			try
			{
				file->writeData(stream);
				size += format->read(stream);
			}
			catch (std::exception &exception)
			{
				throw Exception(boost::format(_("Error while reading map file \"%1%\":\n%2%")) % format->fileName() % exception.what());
			}
		}
	}

	size += this->readSignature(istream);

	return size;
}

std::streamsize W3m::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = this->writeHeader(ostream);
	// TODO create new MPQ archive in stream ostream with all corresponding file formats as MpqFile instances
	//size += mpq::Mpq::write(ostream);

	return size;
}

std::streamsize W3m::readHeader(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	int32 fileId;
	wc3lib::read(istream, fileId, size);

	if (fileId != this->fileId())
		throw Exception(boost::format(_("W3m: Unknown file id \"%1%\". Expected \"%2%\".")) % fileId % this->fileId());

	int32 unknown;
	wc3lib::read(istream, unknown, size);
	wc3lib::readString(istream, m_name, size);
	wc3lib::read<int32>(istream, (int32&)m_flags, size);
	wc3lib::read(istream, m_maxPlayers, size);
	std::size_t byteCount = 512 - size; // followed by 00 bytes until the 512 bytes of the header are filled.
	istream.ignore(byteCount);

	return size;
}

std::streamsize W3m::readSignature(InputStream &istream) throw (class Exception)
{
	std::streamsize result = 0;

	if (!istream.eof())
	{
		byte signId[4];
		wc3lib::read(istream, signId, result, 4);

		// footer is optional!
		if (memcmp(signId, "NGIS", sizeof(signId)) == 0)
		{
			m_signature.reset(new byte[signatureSize]);
			wc3lib::read(istream, m_signature[0], result, signatureSize);
		}
	}

	return result;
}

std::streamsize W3m::writeHeader(OutputStream &ostream) const throw (class Exception)
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
