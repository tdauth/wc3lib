/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#include "info.hpp"

namespace wc3lib
{

namespace map
{

Info::Info(class W3m *w3m) : m_w3m(w3m)
{
}

std::streamsize Info::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_version, size);
	
	if (version() != latestFileVersion())
		std::cerr << boost::format(_("Info: Unsupported version %1%. Latest version is %2%.")) % version() % latestFileVersion() << std::endl;
	
	wc3lib::read(istream, m_mapVersion, size);
	wc3lib::read(istream, m_editorVersion, size);
	wc3lib::readString(istream, m_name, size);
	wc3lib::readString(istream, m_author, size);
	wc3lib::readString(istream, m_description, size);
	wc3lib::readString(istream, m_playersRecommended, size);
	wc3lib::read(istream, m_cameraBoundsJASS, size);
	wc3lib::read(istream, m_cameraBounds, size);
	wc3lib::read(istream, m_playableWidth, size);
	wc3lib::read(istream, m_playableHeight, size);
	wc3lib::read<int32>(istream, (int32&)m_flags, size);
	wc3lib::read(istream, m_mainGroundType, size);
	wc3lib::read(istream, m_campaignBackgroundIndex, size);
	wc3lib::readString(istream, m_loadingScreenText, size);
	wc3lib::readString(istream, m_loadingScreenTitle, size);
	wc3lib::readString(istream, m_loadingScreenSubtitle, size);
	wc3lib::read(istream, m_loadingScreenIndex, size);
	wc3lib::readString(istream, m_prologueScreenText, size);
	wc3lib::readString(istream, m_prologueScreenTitle, size);
	wc3lib::readString(istream, m_prologueScreenSubtitle, size);
	
	/// \todo Read entries
	
	return size;
}

std::streamsize Info::write(OutputStream &ostream) const throw (class Exception)
{
	return 0;
}

}

}
