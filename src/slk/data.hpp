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

#ifndef WC3LIB_SLK_DATA_HPP
#define WC3LIB_SLK_DATA_HPP

#include <list>

#include "../format.hpp"
#include "../map/platform.hpp"

namespace wc3lib
{

namespace slk
{

/**
* Default class for reading <Prefix>Data.txt files like WorldEditData.txt or TriggerStrings.txt.
* @todo Should use a Bison file which defines the possible syntax for these files. Maybe we could use Boost's parser library instead.
*/
class Data : public Format<map::byte>
{
	public:
		Data();
		~Data();

		std::streamsize read(std::istream &istream) throw (class Exception);
		std::streamsize write(std::ostream &ostream) const throw (class Exception);

	protected:
		virtual class DataEntry* dataEntry() = 0;

		std::list<class DataEntry*> m_entries;
};

}

}

#endif
