/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#include "triggers.hpp"
#include "w3m.hpp"
#include "triggercategory.hpp"
#include "variable.hpp"
#include "trigger.hpp"

namespace wc3lib
{

namespace map
{

Triggers::Triggers(class W3m *w3m) : m_w3m(w3m)
{
}

std::streamsize Triggers::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	id requiredId = fileId();
	id fileId;
	wc3lib::read(istream, fileId, size);

	if (memcmp(&fileId, &requiredId, sizeof(fileId)) != 0)
		throw Exception(boost::format(_("Triggers: Unknown file id \"%1%\". Expected \"%2%\".")) % fileId % this->fileId());

	wc3lib::read(istream, this->m_version, size);

	if (this->m_version != latestFileVersion())
		throw Exception(boost::format(_("Triggers: Version %1% is not supported (version %2% only).")) % this->version() % latestFileVersion());

	int32 number;
	wc3lib::read(istream, number, size);
	categories().resize(number);

	for (int32 i = 0; i < number; ++i)
	{
		categories()[i].reset(new TriggerCategory(this));
		size += categories()[i]->read(istream);
	}

	wc3lib::read(istream, this->m_unknown0, size);
	wc3lib::read(istream, number, size);
	variables().resize(number);

	for (int32 i = 0; i < number; ++i)
	{
		variables()[i].reset(new Variable(this));
		size += variables()[i]->read(istream);
	}

	wc3lib::read(istream, number, size);
	triggers().resize(number);

	for (int32 i = 0; i < number; ++i)
	{
		triggers()[i].reset(new Trigger(this));
		size += triggers()[i]->read(istream);
	}

	return size;
}

std::streamsize Triggers::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, fileId(), size);
	wc3lib::write(ostream, this->version(), size);
	int32 number = this->categories().size();
	wc3lib::write(ostream, number, size);

	for (int32 i = 0; i < number; ++i)
		size += categories()[i]->write(ostream);

	wc3lib::write(ostream, this->unknown0(), size);
	number = this->variables().size();
	wc3lib::write(ostream, number, size);

	for (int32 i = 0; i < number; ++i)
		size += variables()[i]->write(ostream);

	number = this->triggers().size();
	wc3lib::write(ostream, number, size);

	for (int32 i = 0; i < number; ++i)
		size += triggers()[i]->write(ostream);

	return size;
}

}

}
