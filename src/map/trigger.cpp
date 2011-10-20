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

#include <boost/foreach.hpp>

#include "trigger.hpp"
#include "triggers.hpp"
#include "triggerfunction.hpp"
#include "triggercategory.hpp"

namespace wc3lib
{

namespace map
{

Trigger::Trigger(class Triggers *triggers) : m_triggers(triggers)
{
}

std::streamsize Trigger::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	readString(istream, this->m_name, size);
	readString(istream, this->m_description, size);
	int32 value;
	wc3lib::read(istream, value, size);
	this->m_isEnabled = value;
	wc3lib::read(istream, value, size);
	this->m_isCustomText = value;
	wc3lib::read(istream, value, size);
	this->m_isInitiallyOn = value;
	wc3lib::read(istream, this->m_unknown, size);
	int32 categoryIndex;
	wc3lib::read(istream, categoryIndex, size);
	this->m_category = this->triggers()->categories()[categoryIndex].get();
	int32 functions;
	wc3lib::read(istream, functions, size);
	m_functions.resize(functions);

	for (int32 i = 0; i < functions; ++i)
	{
		this->m_functions[i].reset(new TriggerFunction(this));
		size += this->m_functions[i]->read(istream);
	}

	return size;
}

std::streamsize Trigger::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;

	writeString(ostream, this->name(), size);
	writeString(ostream, this->description(), size);
	int32 value = this->isEnabled();
	wc3lib::write(ostream, value, size);
	value = this->isCustomText();
	wc3lib::write(ostream, value, size);
	value = this->isInitiallyOn();
	wc3lib::write(ostream, value, size);
	wc3lib::write(ostream, this->unknown(), size);
	wc3lib::write(ostream, this->category()->index(), size);
	value = this->functions().size();
	wc3lib::write(ostream, value, size);

	BOOST_FOREACH(Functions::const_reference function, this->functions())
		size += function->write(ostream);

	return size;
}

}

}
