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

#include <boost/foreach.hpp>

#include "customtexttriggers.hpp"

namespace wc3lib
{

namespace map
{

std::streamsize CustomTextTriggers::read(InputStream& istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, m_version, size);
	
	int32 number = 0;
	wc3lib::read(istream, number, size);
	
	this->triggerTexts().resize(number);

	for (int32 i = 0; i < number; ++i)
	{
		int32 length = 0;
		string text;
		wc3lib::read(istream, length, size);
		
		if (length > 0) {
			length--; // including 0-terminating char!
			
			if (length == 0) { // can only be 0-terminating char!
				byte nullChar = 0;
				wc3lib::read<char>(istream, nullChar, size);
				
				if (nullChar != '\0') {
					std::cerr << boost::format(_("Expected 0-terminating character, got \"%1%\".")) % nullChar << std::endl;
				}
			} else {
				wc3lib::readString(istream, text, size); // expects 0-terminating char!
				
				triggerTexts()[i] = text;
			}
		}
	}

	return size;
}

std::streamsize CustomTextTriggers::write(OutputStream& ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, version(), size);
	wc3lib::write<int32>(ostream, boost::numeric_cast<int32>(triggerTexts().size()), size);

	BOOST_FOREACH(TriggerTexts::const_reference value, triggerTexts())
	{
		const int32 length = value.size() > 0 ? boost::numeric_cast<int32>(value.size() + 1) : 0; // including 0-terminating char!
		wc3lib::write<int32>(ostream, length, size);
		
		if (length > 0) {
			wc3lib::writeString(ostream, value, size); // appends 0-terminating char!
		}
	}

	return size;
}

}

}
