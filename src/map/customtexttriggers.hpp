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

#ifndef WC3LIB_MAP_CUSTOMTEXTTRIGGERS_HPP
#define WC3LIB_MAP_CUSTOMTEXTTRIGGERS_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

class CustomTextTriggers : public FileFormat
{
	public:
		typedef std::vector<string> TriggerTexts;

		virtual std::streamsize read(InputStream& istream);
		virtual std::streamsize write(OutputStream& ostream) const;

		virtual const byte* fileTextId() const;
		virtual const byte* fileName() const;
		virtual uint32 latestFileVersion() const;

		TriggerTexts& triggerTexts();
		const TriggerTexts& triggerTexts() const;

	protected:
		TriggerTexts m_triggerTexts;
};

inline const byte* CustomTextTriggers::fileTextId() const
{
	return "";
}

inline const byte* CustomTextTriggers::fileName() const
{
	return "war3map.wct";
}

inline uint32 CustomTextTriggers::latestFileVersion() const
{
	return 0;
}

inline CustomTextTriggers::TriggerTexts& CustomTextTriggers::triggerTexts()
{
	return m_triggerTexts;
}

inline const CustomTextTriggers::TriggerTexts& CustomTextTriggers::triggerTexts() const
{
	return this->m_triggerTexts;
}

}

}

#endif
