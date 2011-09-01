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

#include "customtexttriggers.hpp"

namespace wc3lib
{

namespace map
{

std::streamsize CustomTextTriggers::read(InputStream& istream) throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_version, size);
	int32 number = 0;
	wc3lib::read(istream, number, size);
	this->m_triggerTexts.resize(number);

	for (int32 i = 0; i < number; ++i)
	{
		int32 length = 0;
		wc3lib::read(istream, length, size);
		wc3lib::readString(istream, this->m_triggerTexts[i], size, length);
	}
}

std::streamsize CustomTextTriggers::write(OutputStream& ostream) const throw (Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, version(), size);
	wc3lib::write<int32>(ostream, triggerTexts().size(), size);

	for (int32 i = 0; i < triggerTexts().size(); ++i)
	{
		wc3lib::write<int32>(ostream, triggerTexts()[i].size(), size);
		wc3lib::write(ostream, *triggerTexts()[i].c_str(), size, triggerTexts()[i].size());
	}
}

}

}
