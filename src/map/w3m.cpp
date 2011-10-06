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

std::streamsize W3m::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = this->readHeader(istream);
	size += mpq::Mpq::read(istream);

	BOOST_FOREACH(FileFormats::reference format, fileFormats())
	{
		mpq::MpqFile *file = this->findFile(format->fileName());

		if (file != 0)
		{
			arraystream stream;
			file->writeData(stream);
			size += format->read(stream);
		}
	}

	size += this->readSignature(istream);

	return size;
}

std::streamsize W3m::write(OutputStream &ostream) const throw (class Exception)
{
	return 0;
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
		char8 signId[4];
		wc3lib::read(istream, signId, result, 4);

		// footer is optional!
		if (memcmp(signId, "NGIS", sizeof(signId)) == 0)
		{
			m_signature.reset(new char8[signatureSize]);
			wc3lib::read(istream, m_signature[0], result, signatureSize);
		}
	}

	return result;
}

}

}
