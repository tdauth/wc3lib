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

#ifndef WC3LIB_MAP_CUSTOMUNITS_HPP
#define WC3LIB_MAP_CUSTOMUNITS_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/**
* Warcraft 3 only allows you to customize unit data.
* Later, in Frozen Throne Blizzard introduced general object data modification (\ref CustomObjects).
*/
class CustomUnits : public FileFormat
{
	public:
		class Modification : public Format
		{
			public:
				Modification();
				~Modification();

				std::streamsize read(InputStream &istream) throw (class Exception);
				std::streamsize write(OutputStream &ostream) const throw (class Exception);

			protected:
				std::streamsize readData(InputStream &istream) throw (class Exception);
				std::streamsize writeData(OutputStream &ostream) const throw (class Exception);

				id m_id; // from "Units\UnitMetaData.slk"
				struct Value m_value;
		};

		class Unit : public Format
		{
			public:
				Unit();
				~Unit();

				std::streamsize read(InputStream &istream) throw (class Exception);
				std::streamsize write(OutputStream &ostream) const throw (class Exception);

				bool isOriginal() { return m_customId == 0; };

			protected:
				virtual class Modification* createModification() const;

				id m_originalId;
				id m_customId;
				std::list<class Modification*> m_modifications;
		};

		CustomUnits();
		~CustomUnits();

		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		virtual const char8* fileName() const;
		virtual id fileId() const;
		virtual int32 latestFileVersion() const;

		virtual int32 version() const { return m_version; }

	protected:
		virtual Unit* createUnit() const;

		int32 m_version;
		std::list<class Unit*> m_originalTable;
		std::list<class Unit*> m_customTable;
};

inline const char8* CustomUnits::fileName() const
{
	return "war3map.w3u";
}

inline id CustomUnits::fileId() const
{
	return 0;
}

inline int32 CustomUnits::latestFileVersion() const
{
	return 1;
}

}

}

#endif
