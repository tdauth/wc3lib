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
#include "triggerdata.hpp"
#include "triggerfunction.hpp"
#include "triggercategory.hpp"

namespace wc3lib
{

namespace map
{

Trigger::Trigger() : m_isEnabled(false), m_isCustomText(false), m_isInitiallyOn(false), m_unknown(0), m_category(0)
{
}

std::streamsize Trigger::read(InputStream &istream, const TriggerData &triggerData)
{
	std::streamsize size = 0;
	readString(istream, this->m_name, size);
	readString(istream, this->m_description, size);
	int32 value;
	wc3lib::read(istream, value, size);
	this->m_isEnabled = boost::lexical_cast<bool>(value);
	wc3lib::read(istream, value, size);
	this->m_isCustomText = boost::lexical_cast<bool>(value);
	wc3lib::read(istream, value, size);
	this->m_isInitiallyOn = !boost::lexical_cast<bool>(value); // 0 initially on, 1 not initially on
	wc3lib::read(istream, this->m_unknown, size);
	wc3lib::read(istream, m_category, size);
	int32 functions;
	wc3lib::read(istream, functions, size);
	this->functions().reserve(functions);

	for (int32 i = 0; i < functions; ++i)
	{
		std::unique_ptr<TriggerFunction> function(new TriggerFunction());
		size += function->read(istream, triggerData);
		this->functions().push_back(std::move(function));
	}

	return size;
}

std::streamsize Trigger::write(OutputStream &ostream) const
{
	std::streamsize size = 0;

	writeString(ostream, this->name(), size);
	writeString(ostream, this->description(), size);
	int32 value = this->isEnabled();
	wc3lib::write(ostream, value, size);
	value = this->isCustomText();
	wc3lib::write(ostream, value, size);
	value = !this->isInitiallyOn(); // 0 initially on, 1 not initially on
	wc3lib::write(ostream, value, size);
	wc3lib::write(ostream, this->unknown(), size);
	wc3lib::write(ostream, category(), size);
	value = boost::numeric_cast<int32>(this->functions().size());
	wc3lib::write(ostream, value, size);

	BOOST_FOREACH(Functions::const_reference function, this->functions()) {
		size += function->write(ostream);
    }

	return size;
}

}

}
